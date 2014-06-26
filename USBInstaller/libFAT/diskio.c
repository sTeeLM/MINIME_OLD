/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "stdafx.h"
#include "diskio.h"		/* FatFs lower layer API */
#include <winioctl.h>
#include <stdlib.h>


FS_DISK AllDisk[256]; /* index is pdrv */

#define LOW_SECTOR_SIZE 512

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{

	return AllDisk[pdrv].h_disk != INVALID_HANDLE_VALUE ? RES_OK:RES_NOTRDY;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	return AllDisk[pdrv].h_disk != INVALID_HANDLE_VALUE ? RES_OK:RES_NOTRDY;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	DRESULT res = RES_OK;
	DWORD dwBytesRead;
	LARGE_INTEGER dist, new_dist;

#ifdef _DEBUG
	TCHAR Buffer[1024];
	_sntprintf_s(Buffer, _countof(Buffer), _countof(Buffer) - 1, _T("RD pdrv=%u sector=%u count=%u\n"), 
		 pdrv, sector, count);
	OutputDebugString(Buffer);
#endif

	dist.QuadPart = (LONGLONG)sector * 512;

	if(AllDisk[pdrv].h_disk != INVALID_HANDLE_VALUE) {
		if( FALSE == SetFilePointerEx(AllDisk[pdrv].h_disk, dist, &new_dist, FILE_BEGIN)) {
			res = RES_ERROR;
		} else {
			if( FALSE == ReadFile(AllDisk[pdrv].h_disk, buff, count * LOW_SECTOR_SIZE, &dwBytesRead, NULL) 
				|| dwBytesRead != count * LOW_SECTOR_SIZE) {
				res = RES_ERROR;
			}
		}
	} else {
		res = RES_NOTRDY;
	}
	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	DRESULT res = RES_OK;
	DWORD dwBytesWrite;
	LARGE_INTEGER dist, new_dist;
	DWORD err;

#ifdef _DEBUG
	TCHAR Buffer[1024];
	_sntprintf_s(Buffer, _countof(Buffer), _countof(Buffer) - 1, _T("WR pdrv=%u sector=%u count=%u\n"), 
		 pdrv, sector, count);
	OutputDebugString(Buffer);
#endif

	dist.QuadPart = (LONGLONG)sector * 512;

	if(AllDisk[pdrv].h_disk != INVALID_HANDLE_VALUE) {
		if( FALSE == SetFilePointerEx(AllDisk[pdrv].h_disk, dist, &new_dist, FILE_BEGIN)) {
			res = RES_ERROR;
		} else {
			if( FALSE == WriteFile(AllDisk[pdrv].h_disk, buff, count * LOW_SECTOR_SIZE, &dwBytesWrite, NULL) 
				|| dwBytesWrite != count * LOW_SECTOR_SIZE) {
				err = GetLastError();
				res = RES_ERROR;
			}
		}
	} else {
		res = RES_NOTRDY;
	}
	return res;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;
	DISK_GEOMETRY_EX geometry;
	LONGLONG tmp = 4294967295L;
	LARGE_INTEGER size;
	DWORD count;

//#define CTRL_SYNC			0	/* Flush disk cache (for write functions) */
//#define GET_SECTOR_COUNT	1	/* Get media size (for only f_mkfs()) */
//#define GET_SECTOR_SIZE		2	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
//#define GET_BLOCK_SIZE		3	/* Get erase block size (for only f_mkfs()) */
//#define CTRL_ERASE_SECTOR	4	/* Force erased a block of sectors (for only _USE_ERASE) */

	if(AllDisk[pdrv].h_disk != INVALID_HANDLE_VALUE) {
		switch(cmd) {
			case CTRL_SYNC: // nothing
			break;
			case GET_SECTOR_SIZE: {
				DWORD *p = (DWORD*)buff;
				*p = LOW_SECTOR_SIZE;
			}break;
			case GET_SECTOR_COUNT: {
				DWORD *p = (DWORD*)buff;
				if(GetFileSizeEx(AllDisk[pdrv].h_disk, &size)) {
					tmp = tmp * LOW_SECTOR_SIZE;
					if(size.QuadPart <= tmp ) {
						*p = (DWORD)(size.QuadPart / LOW_SECTOR_SIZE);
					} else {
						*p = 4294967295;
					}
				} else if (DeviceIoControl(AllDisk[pdrv].h_disk, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0,
					&geometry, sizeof(geometry), &count, NULL)){ // try DeviceIoControl
					tmp = tmp * LOW_SECTOR_SIZE;
					if(geometry.DiskSize.QuadPart <= tmp ) {
						*p = (DWORD)(geometry.DiskSize.QuadPart / LOW_SECTOR_SIZE);
					} else {
						*p = 4294967295;
					}

				} else {
					DWORD err = GetLastError();
					res = RES_ERROR;
				}
			}
			break;
			case GET_BLOCK_SIZE:{
				DWORD *p = (DWORD*)buff;
				*p = LOW_SECTOR_SIZE;
			}
			break;
			case CTRL_ERASE_SECTOR:
			default:
				res = RES_PARERR;
		}
	} else {
		res = RES_NOTRDY;
	}
	return res;
}
#endif
