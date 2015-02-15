
// USBInstaller.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "../libFAT/libFAT.h"

typedef struct usb_device_info
{
	TCHAR	volume;
	TCHAR	friendname[256];
	int		device_num;
	DISK_GEOMETRY_EX geometry;
	HANDLE  hdevice;
	HANDLE  hvolume;
	BOOL	usable;
}usb_device_info_t;

// CUSBInstallerApp:
// 有关此类的实现，请参阅 USBInstaller.cpp
//

class CUSBInstallerApp : public CWinAppEx
{
public:
	CUSBInstallerApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现
private:
	BOOL InitializeDevice();
	virtual int ExitInstance();
	int GetUsbDeviceList(usb_device_info *usb_list, int list_size);
	int GetUsbDeviceFriendName(usb_device_info *usb_list, int list_size);
	int GetUsbDeviceGeometry(usb_device_info *usb_list, int list_size);
	DECLARE_MESSAGE_MAP()
public:
	usb_device_info_t m_UsbList[8];
	int m_UsbCount;
};

extern CUSBInstallerApp theApp;