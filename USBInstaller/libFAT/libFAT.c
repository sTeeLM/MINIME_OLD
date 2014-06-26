// libFAT.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <stdlib.h>

#include "libFAT.h"
#include "ff.h"
#include "diskio.h"
#include "malloc.h"


PARTITION VolToPart[_VOLUMES];
//= {
	//{0, 0},	/* "0:" <== Disk# 0, auto detect */
	//{1, 0},	/* "1:" <== Disk# 1, auto detect */
	//{2, 0},	/* "2:" <== Disk# 2, auto detect */
	//{3, 1},	/* "3:" <== Disk# 3, 1st partition */
	//{3, 2},	/* "4:" <== Disk# 3, 2nd partition */
	//{3, 3},	/* "5:" <== Disk# 3, 3rd partition */
	//{4, 0},	/* "6:" <== Disk# 4, auto detect */
	//{5, 0}	/* "7:" <== Disk# 5, auto detect */
//};

FATFS *FatFs[_VOLUMES];

DWORD get_fattime()
{
	SYSTEMTIME tm;

	/* Get local time */
	GetLocalTime(&tm);

	/* Pack date and time into a DWORD variable */
	return 	  ((DWORD)(tm.wYear - 1980) << 25)
			| ((DWORD)tm.wMonth << 21)
			| ((DWORD)tm.wDay << 16)
			| (WORD)(tm.wHour << 11)
			| (WORD)(tm.wMinute << 5)
			| (WORD)(tm.wSecond >> 1);
}

LIBFAT_API FAT_RESULT FAT_initilize()
{
	int i;
	for(i = 0 ; i < 255 ; i ++) {
		AllDisk[i].h_disk = INVALID_HANDLE_VALUE;
	}
	return FR_OK;
}

LIBFAT_API FAT_RESULT FAT_open(FAT_FP * fp, LPCTSTR path, BYTE mode) /* Open or create a file */
{
	FAT_FP p = NULL;
	FRESULT ret = FR_NO_MEM;

	if(NULL == fp || NULL == path)
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	p = malloc(sizeof(FIL));
	*fp = NULL;
	if(p != NULL) {
		ret = f_open (p,  path, mode);
		if(ret != FR_OK) {
			free(p);
		} else {
			*fp = p; 
		}
	}

	return (FAT_RESULT)ret;
}

LIBFAT_API FAT_RESULT FAT_close (FAT_FP fp)					/* Close an open file object */
{

	FRESULT ret = FR_INVALID_PARAMETER;

	if(NULL == fp)
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	ret = f_close((FIL*)fp);

	if(ret == FR_OK) {
		free(fp);
	}
	return (FAT_RESULT)ret;
}

LIBFAT_API FAT_RESULT FAT_read (FAT_FP fp, void* buff, UINT btr, UINT* br)			/* Read data from a file */
{
	UINT tmp;

	if(NULL == fp || NULL == buff)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	
	if(NULL == br) {
		br = &tmp;
	}

	return (FAT_RESULT)f_read((FIL*)fp, buff, btr, br);
}

LIBFAT_API FAT_RESULT FAT_write (FAT_FP fp, const void* buff, UINT btw, UINT* bw)	/* Write data to a file */
{
	UINT tmp;

	if(NULL == fp || NULL == buff)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	
	if(NULL == bw) {
		bw = &tmp;
	}
	return (FAT_RESULT)f_write((FIL*)fp, buff, btw, bw);
}


LIBFAT_API FAT_RESULT FAT_lseek (FAT_FP fp, DWORD ofs)								/* Move file pointer of a file object */
{
	if(NULL == fp)
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	return (FAT_RESULT)f_lseek((FIL*)fp, ofs);
}

LIBFAT_API FAT_RESULT FAT_truncate (FAT_FP fp)										/* Truncate file */
{
	if(NULL == fp)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_truncate((FIL*)fp);
}

LIBFAT_API FAT_RESULT FAT_sync (FAT_FP fp)											/* Flush cached data of a writing file */
{
	if(NULL == fp)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_sync((FIL*)fp);
}

LIBFAT_API FAT_RESULT FAT_opendir (FAT_DP * dp, LPCTSTR path)						/* Open a directory */
{
	if(NULL == dp || NULL == path )
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_opendir((DIR*)dp, path);
}

LIBFAT_API FAT_RESULT FAT_closedir (FAT_DP dp)										/* Close an open directory */
{
	if(NULL == dp)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_closedir((DIR*)dp);
}

LIBFAT_API FAT_RESULT FAT_readdir (FAT_DP dp, FAT_FILINFO * fno)							/* Read a directory item */
{
	FILINFO _fno;
	FRESULT ret;

	if(NULL == dp || NULL == fno)
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	memset(&_fno, 0, sizeof(_fno));
	ret = f_readdir((DIR*)dp, &_fno);
	fno->fsize = _fno.fsize;
	fno->fdate = _fno.fdate;
	fno->ftime = _fno.ftime;
	fno->fattrib = _fno.fattrib;
	memcpy(fno->fname,_fno.fname,sizeof(fno->fname));
	fno->lfname = _fno.lfname;
	fno->lfsize = _fno.lfsize;
	return (FAT_RESULT)ret;
}

LIBFAT_API FAT_RESULT FAT_mkdir (LPCTSTR path)								/* Create a sub directory */
{
	if(NULL == path)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_mkdir(path);
}

LIBFAT_API FAT_RESULT FAT_unlink (LPCTSTR path)								/* Delete an existing file or directory */
{
	if(NULL == path)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_unlink(path);
}

LIBFAT_API FAT_RESULT FAT_rename (LPCTSTR path_old, LPCTSTR path_new)	/* Rename/Move a file or directory */
{
	if(NULL == path_old || NULL == path_new)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_rename(path_old, path_new);
}

LIBFAT_API FAT_RESULT FAT_stat (LPCTSTR path, FAT_FILINFO * fno)					/* Get file status */
{
	FILINFO _fno;
	FRESULT ret;

	if(NULL == path || NULL == fno)
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	memset(&_fno, 0, sizeof(_fno));
	ret = f_stat(path, &_fno);
	fno->fsize = _fno.fsize;
	fno->fdate = _fno.fdate;
	fno->ftime = _fno.ftime;
	fno->fattrib = _fno.fattrib;
	memcpy(fno->fname,_fno.fname,sizeof(fno->fname));
	fno->lfname = _fno.lfname;
	fno->lfsize = _fno.lfsize;
	return (FAT_RESULT)ret;
}

LIBFAT_API FAT_RESULT FAT_chmod (LPCTSTR path, BYTE value, BYTE mask)			/* Change attribute of the file/dir */
{
	if(NULL == path)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_chmod(path, value, mask);
}

LIBFAT_API FAT_RESULT FAT_utime (LPCTSTR path, const FAT_FILINFO * fno)			/* Change times-tamp of the file/dir */
{
	FILINFO _fno;
	FRESULT ret;

	if(NULL == path || NULL == fno)
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	memset(&_fno, 0, sizeof(_fno));
	_fno.fsize = fno->fsize;
	_fno.fdate = fno->fdate;
	_fno.ftime = fno->ftime;
	_fno.fattrib = fno->fattrib;
	memcpy(_fno.fname,fno->fname,sizeof(fno->fname));
	_fno.lfname = fno->lfname;
	_fno.lfsize = fno->lfsize;
	ret = f_utime(path, &_fno);
	return (FAT_RESULT)ret;
}

LIBFAT_API FAT_RESULT FAT_chdir (LPCTSTR path)								/* Change current directory */
{
	if(NULL == path)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_chdir(path);
}

LIBFAT_API FAT_RESULT FAT_chdrive (LPCTSTR path)								/* Change current drive */
{
	if(NULL == path)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_chdrive(path);
}

LIBFAT_API FAT_RESULT FAT_getcwd (LPTSTR buff, UINT len)							/* Get current directory */
{
	if(NULL == buff)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_getcwd(buff, len);
}

LIBFAT_API FAT_RESULT FAT_getfree (LPCTSTR path, DWORD* nclst, FAT_FSP * fatfs)	/* Get number of free clusters on the drive */
{
	if(NULL == path || NULL == nclst || NULL == fatfs)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_getfree(path, nclst, (FATFS**)fatfs);
}

LIBFAT_API FAT_RESULT FAT_getlabel (LPCTSTR path, LPTSTR label, DWORD* vsn)	/* Get volume label */
{
	DWORD tmp;
	if(NULL == path || NULL == label)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	if(NULL == vsn) {
		vsn = &tmp;
	}
	return (FAT_RESULT)f_getlabel(path, label, vsn);
}

LIBFAT_API FAT_RESULT FAT_setlabel (LPCTSTR label)							/* Set volume label */
{
	if(NULL == label)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return (FAT_RESULT)f_setlabel(label);
}

LIBFAT_API FAT_RESULT FAT_mount (FAT_FSP * fs, BYTE pd, UINT part, BYTE vol, BYTE opt)			/* Mount a logical drive */
{
    FAT_FSP p = NULL;
	FRESULT ret = FR_NO_MEM;
	TCHAR path[64];

	if(NULL == fs || NULL == path)
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	_sntprintf_s(path, _countof(path), _countof(path) - 1, _T("%d:"), vol);

	p = malloc(sizeof(FAT_FS));
	if(NULL != p) {
		memset(p, 0, sizeof(FAT_FS));
		p->pFS = malloc(sizeof(FATFS));
		p->pd = pd;
		if(NULL != p->pFS) {
			memset(p->pFS, 0, sizeof(FATFS));
			p->path = _tcsdup (path);
			if(NULL != p->path) {
				ret = f_mount((FATFS*)p->pFS, p->pd, part, p->path, opt);
				if(FR_OK == ret || FR_NO_FILESYSTEM == ret) {
					*fs = p;
					AllDisk[pd].refcnt ++;
					return (FAT_RESULT)ret;
				}
			}
		}
	}
	if(NULL != p) {
		if(NULL != p->path) {
			free(p->path);
			p->path = NULL;
		}
		if(NULL != p->pFS) {
			free(p->pFS);
			p->pFS = NULL;
		}
		free(p);
	}
	return (FAT_RESULT)ret;
}

LIBFAT_API FAT_RESULT FAT_unmount (FAT_FSP fs)			                        /* Unmount a logical drive */
{

	FRESULT ret = FR_INVALID_PARAMETER;

	if(NULL == fs)
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	ret = f_unmount (fs->pFS, fs->path);
	if(FR_OK == ret) {
		AllDisk[fs->pd].refcnt --;
		if(NULL != fs->path) {
			free(fs->path);
			fs->path = NULL;
		}
		if(NULL != fs->pFS) {
			free(fs->pFS);
			fs->pFS = NULL;
		}
		free(fs);
	}
	return ret;
}

LIBFAT_API FAT_RESULT FAT_sync_fs(FAT_FSP fs)
{
	if(NULL == fs)
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	return sync_fs(fs->pFS);

}

LIBFAT_API FAT_RESULT FAT_mkfs (BYTE pdrv, BYTE part, UINT au, FAT_FS_TYPE type)				/* Create a file system on the volume */
{
	return (FAT_RESULT)f_mkfs(pdrv, part, 0, au, type);
}
LIBFAT_API FAT_RESULT FAT_getfstype (FAT_FSP fs, FAT_FS_TYPE * ret)								/* get fs type */
{
	if(fs && fs->pFS && ret) {
		FATFS * p = (FATFS *)fs->pFS;
		*ret = (FAT_FS_TYPE) p->fs_type;
		return (FAT_RESULT)FR_OK;
	}
	return (FAT_RESULT)FR_INVALID_PARAMETER;
}

LIBFAT_API FAT_RESULT FAT_fdisk (BYTE pdrv, const DWORD szt[],void* work)			/* Divide a physical drive into some partitions */
{
	DWORD sz = 0;
	BYTE MBR[_MAX_SS];

	if(NULL == szt)
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	if(NULL == work) {
		work = MBR;
	}
	return (FAT_RESULT)f_fdisk(pdrv, szt, work);
}

LIBFAT_API INT FAT_putc (TCHAR c, FAT_FP fp)										/* Put a character to the file */
{
	if(NULL == fp)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return f_putc(c, (FIL*)fp);
}

LIBFAT_API INT FAT_puts (LPCTSTR str, FAT_FP cp)								/* Put a string to the file */
{
	if(NULL == str || NULL == cp)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return f_puts(str, (FIL*)cp);
}

LIBFAT_API LPTSTR FAT_gets (LPTSTR buff, int len, FAT_FP fp)						/* Get a string from the file */
{
	if(NULL == buff || NULL == fp)
		return NULL;
	return f_gets(buff, len, (FIL*)fp);
}

LIBFAT_API FAT_RESULT FAT_attach_disk(HANDLE handle, BYTE pd)
{
	HANDLE h_disk = AllDisk[pd].h_disk;

	if(INVALID_HANDLE_VALUE == handle )
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	if(h_disk != INVALID_HANDLE_VALUE) {
		return (FAT_RESULT)FR_BUSY;
	} else {
		AllDisk[pd].h_disk = handle;
		AllDisk[pd].refcnt = 0;
	}
	return (FAT_RESULT)FR_OK;
}

LIBFAT_API FAT_RESULT FAT_detach_disk(BYTE pd)
{
	if(AllDisk[pd].refcnt <= 0) {
		AllDisk[pd].h_disk = INVALID_HANDLE_VALUE;
		AllDisk[pd].refcnt = 0;
		return (FAT_RESULT)FR_OK;
	} else {
		return (FAT_RESULT)FR_BUSY;
	}
}

LIBFAT_API FAT_RESULT FAT_sectmap(FAT_FP fp, DWORD * sec, UINT * sec_cnt)
{
	if(NULL == fp || NULL == sec || NULL == sec_cnt)
		return (FAT_RESULT)FR_INVALID_PARAMETER;
	return f_sectmap((FIL*)fp, sec, sec_cnt);
}

LIBFAT_API FAT_RESULT FAT_get_begin_sect(FAT_FSP fs, DWORD * ret) /* get begin sector */
{
	FATFS * p;
	if(NULL == fs || NULL == fs->pFS || NULL == ret)
		return (FAT_RESULT)FR_INVALID_PARAMETER;

	p = (FATFS *)fs->pFS;
	*ret = p->volbase;

	return (FAT_RESULT)FR_OK;
}

LIBFAT_API FAT_RESULT FAT_read_phy(BYTE pd, DWORD sec_from, void * sec_buf, UINT secr) /* direct read phy */
{
	return disk_read(pd, sec_buf, sec_from, secr) == RES_OK ? (FAT_RESULT)FR_OK : (FAT_RESULT)FR_OK;FR_DISK_ERR;
}

LIBFAT_API FAT_RESULT FAT_write_phy(BYTE pd, DWORD sec_from, const void * sec_buf, UINT secw)  /* direct write phy */
{
	return disk_write(pd, sec_buf, sec_from, secw) == RES_OK ? (FAT_RESULT)FR_OK : (FAT_RESULT)FR_OK;FR_DISK_ERR;
}

LIBFAT_API FAT_RESULT FAT_geometry(BYTE pdrv, DWORD * total_cyl, DWORD * track_p_cyl, 
								   DWORD * sector_p_track, DWORD * byte_p_sector, DWORD * total_sector) /* get geometry */
{
	return f_geometry(pdrv, total_cyl, track_p_cyl, sector_p_track, byte_p_sector, total_sector);
}

LIBFAT_API FAT_RESULT FAT_getvollabel (BYTE pdrv, BYTE part, LPTSTR label, DWORD* vsn)	/* Get volume label */
{
	if(NULL == label)
		return FATR_INVALID_NAME;
	return f_getvollabel(pdrv, part, label, vsn);
}

LIBFAT_API FAT_RESULT FAT_setvollabel (BYTE pdrv, BYTE part, LPCTSTR label, DWORD vsn)			/* Set volume label */
{
	if(NULL == label)
		return FATR_INVALID_NAME;
	return f_setvollabel(pdrv, part, label, vsn);
}
