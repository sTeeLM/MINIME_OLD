// setboot.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "setboot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <Winioctl.h>

// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

#define SECTOR_SIZE 512
#define PART_TABLE  0x1be
#define PART_SIZE   0x10
#define PART_COUNT 4

#pragma pack(1)
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

void error(TCHAR *msg)
{
	_ftprintf(stderr,_T("%s"), msg);
}

BOOL GetStorageDeviceNumberByHandle(HANDLE handle,
				    const STORAGE_DEVICE_NUMBER * sdn)
{
    BOOL result = FALSE;
    DWORD count;

    if (DeviceIoControl(handle, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL,
			0, (LPVOID) sdn, sizeof(*sdn), &count, NULL)) {
	result = TRUE;
    } else {
		error(_T("GetDriveNumber: DeviceIoControl failed"));
    }

    return (result);
}

int GetBytesPerSector(HANDLE drive)
{
    int result = 0;
    DISK_GEOMETRY g;
    DWORD count;

    if (DeviceIoControl(drive, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0,
			&g, sizeof(g), &count, NULL)) {
		result = g.BytesPerSector;
    }

    return (result);
}

BOOL FixMBR(int driveNum, int partitionNum, int set_uefi, int set_active)
{
    BOOL result = TRUE;
    HANDLE drive;
	struct mbr_entry * p_entry;

    TCHAR driveName[128];

    _stprintf(driveName, _T("\\\\.\\PHYSICALDRIVE%d"), driveNum);

    drive = CreateFile(driveName,
		       GENERIC_READ | GENERIC_WRITE,
		       FILE_SHARE_WRITE | FILE_SHARE_READ,
		       NULL, OPEN_EXISTING, 0, NULL);

    if (drive == INVALID_HANDLE_VALUE) {
		error(_T("Accessing physical drive"));
		result = FALSE;
    }

    if (result) {
	unsigned char sector[SECTOR_SIZE];
	DWORD howMany;

	if (GetBytesPerSector(drive) != SECTOR_SIZE) {
	    _ftprintf(stderr,
			_T("Error: Sector size of this drive is %d; must be %d\n"),
		    GetBytesPerSector(drive), SECTOR_SIZE);
	    result = FALSE;
	}

	if (result) {
	    if (ReadFile(drive, sector, sizeof(sector), &howMany, NULL) == 0) {
		error(_T("Reading raw drive"));
		result = FALSE;
	    } else if (howMany != sizeof(sector)) {
		_ftprintf(stderr,
			_T("Error: ReadFile on drive only got %d of %d bytes\n"),
			(int)howMany, sizeof(sector));
		result = FALSE;
	    }
	}

	// Check that our partition is active if specified (-a)
	if (set_active) {
	    if (sector[PART_TABLE + (PART_SIZE * (partitionNum - 1))] != 0x80) {
		int p;
		for (p = 0; p < PART_COUNT; p++)
		    sector[PART_TABLE + (PART_SIZE * p)] =
			(p == partitionNum - 1 ? 0x80 : 0);
	    }
	}
	p_entry = (struct mbr_entry *)&sector[PART_TABLE];
	if(set_uefi) {
		if(p_entry[partitionNum - 1].type != 0xEF) {
			p_entry[partitionNum - 1].type = 0xEF;
		}
	}

	if (result) {
	    SetFilePointer(drive, 0, NULL, FILE_BEGIN);

	    if (WriteFile(drive, sector, sizeof(sector), &howMany, NULL) == 0) {
			error(_T("Writing MBR"));
			result = FALSE;
	    } else if (howMany != sizeof(sector)) {
			_ftprintf(stderr,
				_T("Error: WriteFile on drive only wrote %d of %d bytes\n"),
				(int)howMany, sizeof(sector));
			result = FALSE;
	    }
	}

	if (!CloseHandle(drive)) {
	    error(_T("CloseFile on drive"));
	    result = FALSE;
	}
	}

    return (result);
}

void usage()
{
	_ftprintf(stderr,_T("setboot.exe Z:\n"));
}

int real_tmain(int argc, _TCHAR* argv[])
{
    TCHAR drive_name[] = _T("\\\\.\\?:");
    TCHAR drive_root[] = _T("?:\\");
	UINT drive_type;
	HANDLE d_handle;
	STORAGE_DEVICE_NUMBER sdn;
	int nRet = 0;


	if(argc != 2 || _tcslen(argv[1]) != 2 || argv[1][1] != _T(':')) {
		usage();
		return 1;
	}

	drive_name[4] = argv[1][0];
	drive_root[0] = argv[1][0];
	drive_type = GetDriveType(drive_root);

    d_handle = CreateFile(drive_name, GENERIC_READ | GENERIC_WRITE,
			  FILE_SHARE_READ | FILE_SHARE_WRITE,
			  NULL, OPEN_EXISTING, 0, NULL);

    if (d_handle == INVALID_HANDLE_VALUE) {
		error(_T("Could not open drive"));
		exit(1);
    }

	if (GetStorageDeviceNumberByHandle(d_handle, &sdn)) {
	    if (!FixMBR(sdn.DeviceNumber, sdn.PartitionNumber, 1, 1)) {
			error(_T("Did not successfully update the MBR; continuing...\n"));
			nRet = 1;
	    }
	} else {
	    error(_T("Could not find device number for updating MBR; continuing...\n"));
		nRet = 1;
	}
	::CloseHandle(d_handle);
	return nRet;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		nRetCode = real_tmain(argc, argv);
	}

	return nRetCode;
}
