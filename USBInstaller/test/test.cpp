// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "test.h"
#include "../libFat/libFAT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

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
		BYTE Buffer[512];
		DWORD Size[4];
		FAT_RESULT ret;
		FAT_FSP fsp, fsp1;
		FAT_FP  f;
		UINT size;
		HANDLE hFile = ::CreateFile(
			_T("D:\\Downloads\\usb.img"),
			GENERIC_READ|GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		ret = FAT_initilize();
		ret = FAT_attach_disk(hFile, 0);

		Size[0] = 122;
		Size[1] = -1;
		Size[2] = 0;
		Size[3] = 0;
		ret = FAT_fdisk(0,Size,Buffer);
		/*LIBFAT_API FAT_RESULT FAT_mkfs (BYTE pdrv, BYTE part, UINT au, FAT_FS_TYPE type)*/
		ret = FAT_mkfs(0, 1, 0,FAT_FS_AUTO);
		ret = FAT_mkfs(0, 2, 0,FAT_FS_AUTO);
		ret = FAT_mount(&fsp, 0, 1, 0,1);
		ret = FAT_mount(&fsp1, 0, 2, 1,1);
		//ret = FAT_mkdir(_T("0:\\a"));
		ret = FAT_mkdir(_T("1:\\b"));
		//ret = FAT_setlabel(_T("0:DATA"));
		//ret = FAT_setlabel(_T("1:MINIME"));
		ret = FAT_open(&f, _T("1:\\b\\测试.txt"), FAT_MODE_READ|FAT_MODE_WRITE|FAT_MODE_CREATE_NEW);
		BYTE buf [1234];
		for(UINT i = 0 ; i < sizeof(buf); i ++) {
			buf[i] = 'A';
		}
		buf[sizeof(buf) - 1] = 0;
		size = sizeof(buf);
		ret = FAT_write(f, buf, size, &size);
		ret = FAT_sync(f);
		DWORD sec[1024]; BYTE sec1 [40960]; TCHAR str[13]; UINT u = 1024;
		memset(sec, 0, sizeof(sec));
		memset(sec1, 0, sizeof(sec1));
		ret = FAT_sectmap(f, sec, &u);
		//ret = FAT_close(f);
		ret = FAT_unmount(fsp);
		ret = FAT_unmount(fsp1);
		ret = FAT_setvollabel(0, 2, _T("MINIME"), 0);
		ret = FAT_getvollabel(0, 2, str, 0);
		for(UINT i = 0 ; i < u; i ++) {
			ret = FAT_read_phy(0, sec[i], &sec1[i * 512],1);
		}
		//LIBFAT_API FAT_RESULT FAT_syslinux_install (BYTE pd, UINT part, BYTE vol, LPCTSTR BootPath);	
		//ret = FAT_syslinux_install(0, 2, 0, _T("\\b"), 0);
		ret = FAT_detach_disk(0);
	}

	return nRetCode;
}
