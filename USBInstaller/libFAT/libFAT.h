// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 LIBFAT_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// LIBFAT_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

#ifndef __LIBFAT__H__
#define __LIBFAT__H__

/*
#ifdef LIBFAT_EXPORTS
#define LIBFAT_API __declspec(dllexport)
#else
#define LIBFAT_API __declspec(dllimport)
#endif
*/

#define LIBFAT_API

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

typedef struct {
	DWORD	fsize;			/* File size */
	WORD	fdate;			/* Last modified date */
	WORD	ftime;			/* Last modified time */
	BYTE	fattrib;		/* Attribute */
	TCHAR	fname[13];		/* Short file name (8.3 format) */
	LPTSTR	lfname;			/* Pointer to the LFN buffer */
	UINT 	lfsize;			/* Size of LFN buffer in TCHAR */
} FAT_FILINFO;

typedef void * FAT_FP;
typedef void * FAT_DP;

typedef struct {
	LPTSTR path;
	void * pFS;
	BYTE pd;
}FAT_FS;

typedef FAT_FS * FAT_FSP;

typedef enum {
	FATR_OK = 0,				/* (0) Succeeded */
	FATR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
	FATR_INT_ERR,				/* (2) Assertion failed */
	FATR_NOT_READY,			/* (3) The physical drive cannot work */
	FATR_NO_FILE,				/* (4) Could not find the file */
	FATR_NO_PATH,				/* (5) Could not find the path */
	FATR_INVALID_NAME,		/* (6) The path name format is invalid */
	FATR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	FATR_EXIST,				/* (8) Access denied due to prohibited access */
	FATR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FATR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FATR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FATR_NOT_ENABLED,			/* (12) The volume has no work area */
	FATR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	FATR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
	FATR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FATR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	FATR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FATR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	FATR_INVALID_PARAMETER,	/* (19) Given parameter is invalid */
	FATR_NO_MEM,			    /* (20) No memory */
	FATR_BUSY				/* (21) fs is busy*/
} FAT_RESULT;

typedef enum {
	FAT_FS_AUTO = 0,
	FAT_FS_FAT12,
	FAT_FS_FAT16,
	FAT_FS_FAT32
}FAT_FS_TYPE;

// init
LIBFAT_API FAT_RESULT FAT_initilize();

// disk api
LIBFAT_API FAT_RESULT FAT_attach_disk(HANDLE handle, BYTE pd);
LIBFAT_API FAT_RESULT FAT_detach_disk(BYTE pd);

#define	FAT_MODE_READ				0x01
#define	FAT_MODE_OPEN_EXISTING	    0x00
#define	FAT_MODE_WRITE			    0x02
#define	FAT_MODE_CREATE_NEW		    0x04
#define	FAT_MODE_CREATE_ALWAYS	    0x08
#define	FAT_MODE_OPEN_ALWAYS		0x10

#define	FAT_AM_RDO	0x01	/* Read only */
#define	FAT_AM_HID	0x02	/* Hidden */
#define	FAT_AM_SYS	0x04	/* System */
#define	FAT_AM_VOL	0x08	/* Volume label */
#define FAT_AM_LFN	0x0F	/* LFN entry */
#define FAT_AM_DIR	0x10	/* Directory */
#define FAT_AM_ARC	0x20	/* Archive */
#define FAT_AM_MASK	0x3F	/* Mask of defined bits */

// general api
LIBFAT_API FAT_RESULT FAT_open(FAT_FP * fp, LPCTSTR path, BYTE mode); /* Open or create a file */
LIBFAT_API FAT_RESULT FAT_close (FAT_FP fp);					/* Close an open file object */
LIBFAT_API FAT_RESULT FAT_read (FAT_FP fp, void* buff, UINT btr, UINT* br);			/* Read data from a file */
LIBFAT_API FAT_RESULT FAT_write (FAT_FP fp, const void* buff, UINT btw, UINT* bw);	/* Write data to a file */
LIBFAT_API FAT_RESULT FAT_lseek (FAT_FP fp, DWORD ofs);								/* Move file pointer of a file object */
LIBFAT_API FAT_RESULT FAT_truncate (FAT_FP fp);										/* Truncate file */
LIBFAT_API FAT_RESULT FAT_sync (FAT_FP fp);											/* Flush cached data of a writing file */
LIBFAT_API FAT_RESULT FAT_opendir (FAT_DP * dp, LPCTSTR path);						/* Open a directory */
LIBFAT_API FAT_RESULT FAT_closedir (FAT_DP dp);										/* Close an open directory */
LIBFAT_API FAT_RESULT FAT_readdir (FAT_DP dp, FAT_FILINFO * fno);							/* Read a directory item */
LIBFAT_API FAT_RESULT FAT_mkdir (LPCTSTR path);								/* Create a sub directory */
LIBFAT_API FAT_RESULT FAT_unlink (LPCTSTR path);								/* Delete an existing file or directory */
LIBFAT_API FAT_RESULT FAT_rename (LPCTSTR path_old, LPCTSTR path_new);	/* Rename/Move a file or directory */
LIBFAT_API FAT_RESULT FAT_stat (LPCTSTR path, FAT_FILINFO * fno);					/* Get file status */
LIBFAT_API FAT_RESULT FAT_chmod (LPCTSTR path, BYTE value, BYTE mask);			/* Change attribute of the file/dir */
LIBFAT_API FAT_RESULT FAT_utime (LPCTSTR path, const FAT_FILINFO * fno);			/* Change times-tamp of the file/dir */
LIBFAT_API FAT_RESULT FAT_chdir (LPCTSTR path);								/* Change current directory */
LIBFAT_API FAT_RESULT FAT_chdrive (LPCTSTR path);								/* Change current drive */
LIBFAT_API FAT_RESULT FAT_getcwd (LPTSTR buff, UINT len);							/* Get current directory */
LIBFAT_API FAT_RESULT FAT_getfree (LPCTSTR path, DWORD* nclst, FAT_FSP * fatfs);	/* Get number of free clusters on the drive */
LIBFAT_API FAT_RESULT FAT_getlabel (LPCTSTR path, LPTSTR label, DWORD* vsn);	/* Get volume label on root dir*/
LIBFAT_API FAT_RESULT FAT_setlabel (LPCTSTR label);							/* Set volume label on root dir */
LIBFAT_API FAT_RESULT FAT_getvollabel (BYTE pdrv, BYTE part, LPTSTR label, DWORD* vsn);	/* Get volume label */
LIBFAT_API FAT_RESULT FAT_setvollabel (BYTE pdrv, BYTE part, LPCTSTR label, DWORD vsn);			/* Set volume label */
LIBFAT_API FAT_RESULT FAT_mount (FAT_FSP * fs, BYTE pd, UINT part, BYTE vol, BYTE opt);			/* Mounta logical drive */
LIBFAT_API FAT_RESULT FAT_unmount (FAT_FSP fs);			                        /* Unmount a logical drive */
LIBFAT_API FAT_RESULT FAT_mkfs (BYTE pdrv, BYTE part, UINT au, FAT_FS_TYPE type);				/* Create a file system on the volume */
LIBFAT_API FAT_RESULT FAT_getfstype (FAT_FSP fs, FAT_FS_TYPE * ret);				/* get fs type */
LIBFAT_API FAT_RESULT FAT_fdisk (BYTE pdrv, const DWORD szt[], void* work);			/* Divide a physical drive into some partitions */
LIBFAT_API INT FAT_putc (TCHAR c, FAT_FP fp);										/* Put a character to the file */
LIBFAT_API INT FAT_puts (LPCTSTR str, FAT_FP cp);								/* Put a string to the file */
LIBFAT_API LPTSTR FAT_gets (LPTSTR buff, int len, FAT_FP fp);						/* Get a string from the file */
LIBFAT_API FAT_RESULT FAT_sectmap(FAT_FP fp, DWORD * sec, UINT * sec_cnt);     /* get sector map*/
LIBFAT_API FAT_RESULT FAT_sync_fs(FAT_FSP fs);  								/* sync fs */
LIBFAT_API FAT_RESULT FAT_read_phy(BYTE pd, DWORD sec_from, void * sec_buf, UINT secr); /* direct read phy */
LIBFAT_API FAT_RESULT FAT_write_phy(BYTE pd, DWORD sec_from, const void * sec_buf, UINT secw);  /* direct write phy */
LIBFAT_API FAT_RESULT FAT_get_begin_sect(FAT_FSP fs, DWORD * ret); /* get begin sector */
LIBFAT_API FAT_RESULT FAT_geometry(BYTE pdrv, DWORD * total_cyl, DWORD * track_p_cyl, 
								   DWORD * sector_p_track, DWORD * byte_p_sector, DWORD * total_sector); /* get geometry */

/* syslinux */
LIBFAT_API LPCTSTR FAT_syslinux_get_version ();
LIBFAT_API FAT_RESULT FAT_syslinux_install (BYTE pd, UINT part, BYTE vol, LPCTSTR BootPath, int efi);	

#ifdef __cplusplus
}
#endif


#endif

