#include "stdafx.h"
#include <stdlib.h> 
#include <stddef.h> 
#include "ff.h"
#include "libFat.h"

extern const unsigned char __ldlinux_sys[];
extern const unsigned int __ldlinux_sys_len;

extern const unsigned char __ldlinux_c32[];
extern unsigned int __ldlinux_c32_len;

extern const unsigned char __ldlinux_bss[];
extern unsigned int __ldlinux_bss_len;

extern const unsigned char __mbr_bin[];
extern unsigned int __mbr_bin_len;

#define FAT_SYSLINUX_VERSION _T("SYSLINUX 5.10")

#define X86_MEM 1

#define SECTOR_SIZE     512
#define SECTOR_SHIFT    9
#define PART_TABLE  0x1be
#define PART_SIZE   0x10
#define PART_COUNT 4

#define ADV_SIZE        512
#define ADV_LEN         (ADV_SIZE-3*4)

typedef DWORD sector_t;

unsigned char syslinux_adv[2 * ADV_SIZE];

unsigned int boot_image_len;
unsigned char * boot_image;

unsigned char * boot_sector;

#define LDLINUX_MAGIC 0x3eb202fe

#define ADV_MAGIC1	0x5a2d2fa5	/* Head signature */
#define ADV_MAGIC2	0xa3041767	/* Total checksum */
#define ADV_MAGIC3	0xdd28bf64	/* Tail signature */

typedef ULONGLONG uint64_t;
typedef DWORD uint32_t;
typedef WORD  uint16_t;
typedef BYTE  uint8_t;

/* Patch area for disk-based installers */
struct patch_area {
    uint32_t magic;		/* LDLINUX_MAGIC */
    uint32_t instance;		/* Per-version value */
    uint16_t data_sectors;
    uint16_t adv_sectors;
    uint32_t dwords;
    uint32_t checksum;
    uint16_t maxtransfer;
    uint16_t epaoffset;		/* Pointer to the extended patch area */
};

struct ext_patch_area {
    uint16_t advptroffset;	/* ADV pointers */
    uint16_t diroffset;		/* Current directory field */
    uint16_t dirlen;		/* Length of current directory field */
    uint16_t subvoloffset;	/* Subvolume field */
    uint16_t subvollen;		/* Length of subvolume field */
    uint16_t secptroffset;	/* Sector extent pointers */
    uint16_t secptrcnt;		/* Number of sector extent pointers */

    uint16_t sect1ptr0;		/* Boot sector offset of sector 1 ptr LSW */
    uint16_t sect1ptr1;		/* Boot sector offset of sector 1 ptr MSW */
    uint16_t raidpatch;		/* Boot sector RAID mode patch pointer */
};

/* Sector extent */
#pragma pack(push, r1, 1)
struct syslinux_extent {
    uint64_t lba;
    uint16_t len;
};
#pragma pack(pop, r1)

/* mbr */
#pragma pack(push, r1, 1)
struct mbr_entry
{
    /* status / physical drive (bit 7 set: active / bootable,
    old MBRs only accept 80h), 00h: inactive, 01h"C7Fh: invalid) */
    BYTE status;

    /* CHS address of first absolute sector in partition.
    The format is described by 3 bytes, see the next 3 rows.
    0 bytes: h[7"C0]  head
    1 bytes: c[9-8] s[0-5] sector
    2 bytes: c[7"C0]
    */
    BYTE chs_begin_head;
    WORD chs_begin_cylinder_sector;

    /* Partition type[13] */
    BYTE type;

    /* CHS address of last absolute sector in partition.
    The format is described by 3 bytes, see the next 3 rows.*/
    BYTE chs_end_head;
    WORD chs_end_cylinder_sector;

    /* LBA of first absolute sector in the partition */
    DWORD lba_begin;

    /* Number of sectors in partition */
    DWORD lba_size;

};
#pragma pack(pop, r1)

/* FAT bootsector format, also used by other disk-based derivatives */
#pragma pack(push, r1, 1)
struct fat_boot_sector {
    uint8_t bsJump[3];
    char bsOemName[8];
    uint16_t bsBytesPerSec;
    uint8_t bsSecPerClust;
    uint16_t bsResSectors;
    uint8_t bsFATs;
    uint16_t bsRootDirEnts;
    uint16_t bsSectors;
    uint8_t bsMedia;
    uint16_t bsFATsecs;
    uint16_t bsSecPerTrack;
    uint16_t bsHeads;
    uint32_t bsHiddenSecs;
    uint32_t bsHugeSectors;

	#pragma pack(push, r2, 1)
    union {
	#pragma pack(push, r3, 1)
	struct {
	    uint8_t DriveNumber;
	    uint8_t Reserved1;
	    uint8_t BootSignature;
	    uint32_t VolumeID;
	    char VolumeLabel[11];
	    char FileSysType[8];
	    uint8_t Code[442];
	}bs16;
	#pragma pack(pop, r3)
	#pragma pack(push, r3, 1)
	struct {
	    uint32_t FATSz32;
	    uint16_t ExtFlags;
	    uint16_t FSVer;
	    uint32_t RootClus;
	    uint16_t FSInfo;
	    uint16_t BkBootSec;
	    uint8_t Reserved0[12];
	    uint8_t DriveNumber;
	    uint8_t Reserved1;
	    uint8_t BootSignature;
	    uint32_t VolumeID;
	    char VolumeLabel[11];
	    char FileSysType[8];
	    uint8_t Code[414];
	}bs32;
	#pragma pack(pop, r3)
    };
	#pragma pack(pop, r2)

    uint32_t bsMagic;
    uint16_t bsForwardPtr;
    uint16_t bsSignature;
};
#pragma pack(pop, r1)

#define FAT_bsHead      bsJump
#define FAT_bsHeadLen   offsetof(struct fat_boot_sector, bsBytesPerSec)
#define FAT_bsCode	    bs32.Code	/* The common safe choice */
#define FAT_bsCodeLen   (offsetof(struct fat_boot_sector, bsSignature) - \
		     offsetof(struct fat_boot_sector, FAT_bsCode))

/*
 * Access functions for littleendian numbers, possibly misaligned.
 */
static uint8_t get_8(const uint8_t * p)
{
    return *p;
}

static uint16_t get_16(const uint16_t * p)
{
#if X86_MEM
    /* Littleendian and unaligned-capable */
    return *p;
#else
    const uint8_t *pp = (const uint8_t *)p;
    return pp[0] + ((uint16_t)pp[1] << 8);
#endif
}

static uint32_t get_32(const uint32_t * p)
{
#if X86_MEM
    /* Littleendian and unaligned-capable */
    return *p;
#else
    const uint16_t *pp = (const uint16_t *)p;
    return get_16(pp[0]) + (uint32_t)get_16(pp[1]);
#endif
}

static uint64_t get_64(const uint64_t * p)
{
#if X86_MEM
    /* Littleendian and unaligned-capable */
    return *p;
#else
    const uint32_t *pp = (const uint32_t *)p;
    return get_32(pp[0]) + (uint64_t)get_32(pp[1]);
#endif
}

static void set_8(uint8_t *p, uint8_t v)
{
    *p = v;
}

static void set_16(uint16_t *p, uint16_t v)
{
#if X86_MEM
    /* Littleendian and unaligned-capable */
    *p = v;
#else
    uint8_t *pp = (uint8_t *) p;
    pp[0] = (v & 0xff);
    pp[1] = ((v >> 8) & 0xff);
#endif
}

static void set_32(uint32_t *p, uint32_t v)
{
#if X86_MEM
    /* Littleendian and unaligned-capable */
    *p = v;
#else
    uint8_t *pp = (uint8_t *) p;
    pp[0] = (v & 0xff);
    pp[1] = ((v >> 8) & 0xff);
    pp[2] = ((v >> 16) & 0xff);
    pp[3] = ((v >> 24) & 0xff);
#endif
}

static void set_64(uint64_t *p, uint64_t v)
{
#if X86_MEM
    /* Littleendian and unaligned-capable */
    *p = v;
#else
    uint32_t *pp = (uint32_t *) p;
    set_32(pp[0], v);
    set_32(pp[1], v >> 32);
#endif
}

#define get_8_sl(x)    		get_8(x)
#define get_16_sl(x)   		get_16(x)
#define get_32_sl(x)   		get_32(x)
#define get_64_sl(x)   		get_64(x)
#define set_8_sl(x,y)  		set_8(x,y)
#define set_16_sl(x,y) 		set_16(x,y)
#define set_32_sl(x,y) 		set_32(x,y)
#define set_64_sl(x,y) 		set_64(x,y)
#define memcpy_to_sl(d,s,l)	memcpy(d,s,l)
#define memcpy_from_sl(d,s,l)	memcpy(d,s,l)

static void cleanup_adv(unsigned char *advbuf)
{
    int i;
    uint32_t csum;

    /* Make sure both copies agree, and update the checksum */
    set_32((uint32_t *) advbuf, ADV_MAGIC1);

    csum = ADV_MAGIC2;
    for (i = 8; i < ADV_SIZE - 4; i += 4)
	csum -= get_32((uint32_t *) (advbuf + i));

    set_32((uint32_t *) (advbuf + 4), csum);
    set_32((uint32_t *) (advbuf + ADV_SIZE - 4), ADV_MAGIC3);

    memcpy(advbuf + ADV_SIZE, advbuf, ADV_SIZE);
}


static void syslinux_reset_adv(unsigned char *advbuf)
{
    /* Create an all-zero ADV */
    memset(advbuf + 2 * 4, 0, ADV_LEN);
    cleanup_adv(advbuf);
}

/*
 * Generate sector extents
 */
static void generate_extents(struct syslinux_extent *ex, int nptrs,
			     const sector_t *sectp, int nsect)
{
    uint32_t addr = 0x8000;	/* ldlinux.sys starts loading here */
    uint32_t base;
    sector_t sect, lba;
    unsigned int len;

    base = addr;
    len = lba = 0;

    memset(ex, 0, nptrs * sizeof *ex);

    while (nsect) {
	sect = *sectp++;

	if (len) {
	    uint32_t xbytes = (len + 1) * SECTOR_SIZE;

	    if (sect == lba + len && xbytes < 65536 &&
		((addr ^ (base + xbytes - 1)) & 0xffff0000) == 0) {
		/* We can add to the current extent */
		len++;
		goto next;
	    }

	    set_64_sl(&ex->lba, lba);
	    set_16_sl(&ex->len, len);
	    ex++;
	}

	base = addr;
	lba  = sect;
	len  = 1;

    next:
	addr += SECTOR_SIZE;
	nsect--;
    }

    if (len) {
	set_64_sl(&ex->lba, lba);
	set_16_sl(&ex->len, len);
	ex++;
    }
}


/*
 * Form a pointer based on a 16-bit patcharea/epa field
 */
static void *ptr(void *img, uint16_t *offset_p)
{
    return (char *)img + get_16_sl(offset_p);
}

/*
 * This patches the boot sector and the beginning of ldlinux.sys
 * based on an ldlinux.sys sector map passed in.  Typically this is
 * handled by writing ldlinux.sys, mapping it, and then overwrite it
 * with the patched version.  If this isn't safe to do because of
 * an OS which does block reallocation, then overwrite it with
 * direct access since the location is known.
 *
 * Returns the number of modified bytes in ldlinux.sys if successful,
 * otherwise -1.
 */
int syslinux_patch(const sector_t *sectp, int nsectors,
		   int stupid, int raid_mode,
		   const char *subdir, const char *subvol)
{
    struct patch_area *patcharea;
    struct ext_patch_area *epa;
    struct syslinux_extent *ex;
    uint32_t *wp;
    int nsect = ((boot_image_len + SECTOR_SIZE - 1) >> SECTOR_SHIFT) + 2;
    uint32_t csum;
    int i, dw, nptrs;
    struct fat_boot_sector *sbs = (struct fat_boot_sector *)boot_sector;
    uint64_t *advptrs;

    if (nsectors < nsect)
	return -1;		/* The actual file is too small for content */

    /* Search for LDLINUX_MAGIC to find the patch area */
    for (wp = (uint32_t *)boot_image; get_32_sl(wp) != LDLINUX_MAGIC;
	 wp++)
	;
    patcharea = (struct patch_area *)wp;
    epa = ptr(boot_image, &patcharea->epaoffset);

    /* First sector need pointer in boot sector */
    set_32(ptr(sbs, &epa->sect1ptr0), sectp[0]);
//    set_32(ptr(sbs, &epa->sect1ptr1), sectp[0] >> 32);
	set_32(ptr(sbs, &epa->sect1ptr1), 0);
    sectp++;

    /* Handle RAID mode */
    if (raid_mode) {
	/* Patch in INT 18h = CD 18 */
	set_16(ptr(sbs, &epa->raidpatch), 0x18CD);
    }

    /* Set up the totals */
    dw = boot_image_len >> 2;	/* COMPLETE dwords, excluding ADV */
    set_16_sl(&patcharea->data_sectors, nsect - 2); /* Not including ADVs */
    set_16_sl(&patcharea->adv_sectors, 2);	/* ADVs need 2 sectors */
    set_32_sl(&patcharea->dwords, dw);

    /* Handle Stupid mode */
    if (stupid) {
	/* Access only one sector at a time */
	set_16_sl(&patcharea->maxtransfer, 1);
    }

    /* Set the sector extents */
    ex = ptr(boot_image, &epa->secptroffset);
    nptrs = get_16_sl(&epa->secptrcnt);

#if 0
    if (nsect > nptrs) {
	/* Not necessarily an error in this case, but a general problem */
	fprintf(stderr, "Insufficient extent space, build error!\n");
	exit(1);
    }
#endif

    /* -1 for the pointer in the boot sector, -2 for the two ADVs */
    generate_extents(ex, nptrs, sectp, nsect-1-2);

    /* ADV pointers */
    advptrs = ptr(boot_image, &epa->advptroffset);
    set_64_sl(&advptrs[0], sectp[nsect-1-2]);
    set_64_sl(&advptrs[1], sectp[nsect-1-1]);

    /* Poke in the base directory path */
    if (subdir) {
	int sublen = strlen(subdir) + 1;
	if (get_16_sl(&epa->dirlen) < sublen) {
	    //fprintf(stderr, "Subdirectory path too long... aborting install!\n");
	    return -1;
	}
	memcpy_to_sl(ptr(boot_image, &epa->diroffset), subdir, sublen);
    }

    /* Poke in the subvolume information */
    if (subvol) {
	int sublen = strlen(subvol) + 1;
	if (get_16_sl(&epa->subvollen) < sublen) {
	    //fprintf(stderr, "Subvol name too long... aborting install!\n");
	    return -1;
	}
	memcpy_to_sl(ptr(boot_image, &epa->subvoloffset), subvol, sublen);
    }

    /* Now produce a checksum */
    set_32_sl(&patcharea->checksum, 0);

    csum = LDLINUX_MAGIC;
    for (i = 0, wp = (uint32_t *)boot_image; i < dw; i++, wp++)
	csum -= get_32_sl(wp);	/* Negative checksum */

    set_32_sl(&patcharea->checksum, csum);

    /*
     * Assume all bytes modified.  This can be optimized at the expense
     * of keeping track of what the highest modified address ever was.
     */
    return dw << 2;
}

static void syslinux_make_bootsect(void *bs)
{
	struct fat_boot_sector *bootsect = bs;
	const struct fat_boot_sector *sbs =
	    (const struct fat_boot_sector *)boot_sector;

	memcpy(&bootsect->FAT_bsHead, &sbs->FAT_bsHead, FAT_bsHeadLen);
	memcpy(&bootsect->FAT_bsCode, &sbs->FAT_bsCode, FAT_bsCodeLen);
}

LIBFAT_API LPCTSTR FAT_syslinux_get_version ()
{
	return FAT_SYSLINUX_VERSION;
}

static LPSTR W2A(LPCWSTR str)
{
	LPSTR ret = NULL;
	INT len = 0;
	len = WideCharToMultiByte(
		CP_OEMCP,
		0,
		str,
		-1,
		NULL,
		0,
		NULL,
		NULL);
	if(len > 0) {
		ret = malloc(len + 1);
		if(NULL != ret) {
			len = WideCharToMultiByte(
				CP_OEMCP,
				0,
				str,
				-1,
				ret,
				len + 1,
				NULL,
				NULL);
			if(len > 0) {
				ret[len] = 0;
				return ret;
			} else {
				free(ret);
				ret = NULL;
			}
		}
	}
	return NULL;
}

LIBFAT_API FAT_RESULT FAT_syslinux_install (BYTE pd, UINT part, BYTE vol, LPCTSTR BootPath, int efi)
{
	FAT_FP fp_ld_linux_sys = NULL;
	FAT_FP fp_ld_linux_c32 = NULL;
	FAT_FSP fs = NULL;
	FAT_RESULT ret = (FAT_RESULT)FR_NO_MEM;
	TCHAR path[4096];
	CHAR * aPath = NULL;
	size_t len;
	UINT ulen;
	DWORD * pSectors = NULL;
	unsigned char * p_boot_image = NULL;
	unsigned char * p_boot_sector = NULL;
	int patch_sectors;
    int i;
	UINT j;
	unsigned char sectbuf[SECTOR_SIZE];
	unsigned char mbr[SECTOR_SIZE];
	struct mbr_entry * p_entry;
	DWORD dwVolOff = 0;

	if(NULL == BootPath)
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	// install  ldlinux.c32
	len = _tcslen(BootPath);
	if(len < 1 || BootPath[0] != _T('\\'))
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	if((ret = FAT_mount(&fs, pd, part, vol, 1)) != FR_OK) {
		goto err;
	}

	if(BootPath[len - 1] == _T('\\'))
		_sntprintf_s(path, _countof(path), _countof(path) - 1, _T("%d:%s%s"), vol, BootPath, _T("ldlinux.c32"));
	else
		_sntprintf_s(path, _countof(path), _countof(path) - 1, _T("%d:%s\\%s"), vol, BootPath, _T("ldlinux.c32"));

	if((ret = FAT_open(&fp_ld_linux_c32, path, FA_READ | FA_WRITE | FA_CREATE_ALWAYS)) != FR_OK) {
		goto err;
	}

	if((FAT_truncate(fp_ld_linux_c32)) != FR_OK) {
		goto err;
	}

	ulen = __ldlinux_c32_len;
	if((ret = FAT_write(fp_ld_linux_c32, __ldlinux_c32, ulen, &ulen) != FR_OK || ulen != __ldlinux_c32_len)) goto err;
	FAT_close(fp_ld_linux_c32);
	fp_ld_linux_c32 = NULL;

	if((FAT_chmod(path, FAT_AM_RDO|FAT_AM_HID|FAT_AM_SYS, FAT_AM_RDO|FAT_AM_HID|FAT_AM_SYS)) != FR_OK) {
		goto err;
	}

	// install ldlinux.sys
	if(BootPath[len - 1] == _T('\\'))
		_sntprintf_s(path, _countof(path), _countof(path) - 1, _T("%d:%s%s"), vol, BootPath, _T("ldlinux.sys"));
	else
		_sntprintf_s(path, _countof(path), _countof(path) - 1, _T("%d:%s\\%s"), vol, BootPath, _T("ldlinux.sys"));

	// alloc buffer
	ulen = __ldlinux_sys_len;
	// scal to SECTOR_SIZE
	ulen = ((ulen + (SECTOR_SIZE - 1)) >> SECTOR_SHIFT) << SECTOR_SHIFT;
	
	boot_image_len = ulen;

	if((p_boot_image = malloc(ulen)) == NULL) {
		ret = (FAT_RESULT)FR_NO_MEM;
		goto err;
	}
	memset(p_boot_image, 0, ulen);
	memcpy(p_boot_image, __ldlinux_sys, __ldlinux_sys_len);

	boot_image = p_boot_image;

	if((ret = FAT_open(&fp_ld_linux_sys, path, FA_READ | FA_WRITE | FA_CREATE_ALWAYS)) != FR_OK) {
		goto err;
	}

	if((FAT_truncate(fp_ld_linux_sys)) != FR_OK) {
		goto err;
	}

	if((ret = FAT_write(fp_ld_linux_sys, p_boot_image, ulen, &ulen)) != FR_OK || ulen != boot_image_len) {
		if(FR_OK == ret)
			ret = (FAT_RESULT)FR_INT_ERR;
		goto err;
	}
	
	// write adv
	ulen = sizeof(syslinux_adv);
	syslinux_reset_adv(syslinux_adv);
	if((ret = FAT_write(fp_ld_linux_sys, syslinux_adv, ulen ,&ulen)) != FR_OK || ulen != sizeof(syslinux_adv)) {
		if(FR_OK == ret)
			ret = (FAT_RESULT)FR_INT_ERR;
		goto err;
	}

	// sync to disk
	if((ret = FAT_sync(fp_ld_linux_sys)) != FR_OK) goto err;
	ulen = boot_image_len / SECTOR_SIZE + 2; // ld_linux_sys + adv

	if((pSectors = malloc(ulen * sizeof(DWORD))) == NULL) {
		ret = (FAT_RESULT)FR_NO_MEM;
		goto err;
	}

	// get sector map
	if ((ret = FAT_sectmap(fp_ld_linux_sys, pSectors, &ulen))  != FR_OK) {
		goto err;
	}
	FAT_close(fp_ld_linux_sys);
	fp_ld_linux_sys = NULL;


	if((FAT_chmod(path, FAT_AM_RDO|FAT_AM_HID|FAT_AM_SYS, FAT_AM_RDO|FAT_AM_HID|FAT_AM_SYS)) != FR_OK) {
		goto err;
	}


#ifdef _DEBUG
	{
		TCHAR Buffer[1024];
		UINT j;
		for( j = 0 ; j < ulen; j ++) {
			_sntprintf_s(Buffer, _countof(Buffer), _countof(Buffer) - 1, _T("SECMAP SCALED %d\n"), pSectors[j]);
			OutputDebugString(Buffer);
		}
	}
#endif

	FAT_sync_fs(fs);

	if((ret = FAT_get_begin_sect(fs, &dwVolOff)) != FR_OK) {
		goto err;
	}

	for(j = 0 ; j < ulen; j ++) {
		pSectors[j]-= dwVolOff;
	}

	{
		TCHAR Buffer[1024];
		UINT j;
		for( j = 0 ; j < ulen; j ++) {
			_sntprintf_s(Buffer, _countof(Buffer), _countof(Buffer) - 1, _T("SECMAP SCALED %d\n"), pSectors[j]);
			OutputDebugString(Buffer);
		}
	}

	// unmount fs
	FAT_unmount(fs);
	fs = NULL;

	if(NULL == (p_boot_sector = malloc(__ldlinux_bss_len))) {
		ret = (FAT_RESULT)FR_NO_MEM;
		goto err;
	}

	memcpy(p_boot_sector, __ldlinux_bss, __ldlinux_bss_len);
	boot_sector = p_boot_sector;

	// Patch ldlinux.sys and the boot sector
	if(BootPath[len - 1] == _T('\\'))
		_sntprintf_s(path, _countof(path), _countof(path) - 1, _T("%s"), BootPath);
	else
		_sntprintf_s(path, _countof(path), _countof(path) - 1, _T("%s\\"), BootPath);

#ifdef _UNICODE
	if((aPath = W2A(path)) == NULL) {
		ret = (FAT_RESULT)FR_INT_ERR;
		goto err;
	}
    i = syslinux_patch(pSectors, ulen, 0, 0, aPath, NULL);
#else
	i = syslinux_patch(pSectors, ulen, 0, 0, path, NULL);
#endif
    patch_sectors = (i + SECTOR_SIZE - 1) >> SECTOR_SHIFT;
	
	if(i < 0) {
		ret = (FAT_RESULT)FR_INT_ERR;
		goto err;
	}

	if(NULL != aPath) {
		free(aPath);
		aPath = NULL;
	}

#ifdef _DEBUG
	{
		TCHAR Buffer[1024];
		UINT j;
		for( j = 0 ; j < ulen; j ++) {
			_sntprintf_s(Buffer, _countof(Buffer), _countof(Buffer) - 1, _T("SECMAP AFTER PATCH %d\n"), pSectors[j]);
			OutputDebugString(Buffer);
		}
	}
#endif
    for (i = 0; i < patch_sectors; i++) {
		if((ret = FAT_write_phy(pd, dwVolOff + pSectors[i], boot_image + i * SECTOR_SIZE, 1)) != FR_OK)
			goto err;
    }
	
	// read boot sector again
	if((ret = FAT_read_phy(pd, dwVolOff, sectbuf, 1)) != FR_OK) goto err;

	syslinux_make_bootsect(sectbuf);

	if((ret = FAT_write_phy(pd, dwVolOff, sectbuf, 1)) != FR_OK) goto err;

	// read mbr
	if((ret = FAT_read_phy(pd, 0, mbr, 1)) != FR_OK) goto err;

	// set active
	memcpy(mbr, __mbr_bin, __mbr_bin_len);

	if (mbr[PART_TABLE + (PART_SIZE * (part - 1))] != 0x80) {
		for (i = 0; i < PART_COUNT; i++)
			mbr[PART_TABLE + (PART_SIZE * i)] = (i == part - 1 ? 0x80 : 0);
	}

	// support efi boot

	if(efi) {
		p_entry = (struct mbr_entry *)&mbr[PART_TABLE];
		if(p_entry[part - 1].type != 0xEF) {
				p_entry[part - 1].type = 0xEF;
		}
	}

	// write mbr
	if((ret = FAT_write_phy(pd, 0, mbr, 1)) != FR_OK) goto err;

	free(p_boot_image);
	p_boot_image = NULL;
	boot_image = NULL;
	boot_image_len = 0;

	free(p_boot_sector);
	p_boot_sector = NULL;
	boot_sector = NULL;

	free(pSectors);
	pSectors = NULL;

	ret =  FR_OK;
err:
	if(NULL != fs) {
		FAT_unmount(fs);
		fs = NULL;
	}
	if(NULL != fp_ld_linux_c32) {
		FAT_close(fp_ld_linux_c32);
		fp_ld_linux_c32 = NULL;
	}
	if(NULL != fp_ld_linux_sys) {
		FAT_close(fp_ld_linux_sys);
		fp_ld_linux_sys = NULL;
	}
	if(NULL != p_boot_image) {
		free(p_boot_image);
		p_boot_image = NULL;
		boot_image = NULL;
		boot_image_len = 0;

	}
	if(NULL != p_boot_sector) {
		free(p_boot_sector);
		p_boot_sector = NULL;
		boot_sector = NULL;
	}
	if(NULL != pSectors) {
		free(pSectors);
		pSectors = NULL;
	}
	if(NULL != aPath) {
		free(aPath);
		aPath = NULL;
	}
	return ret;
}
