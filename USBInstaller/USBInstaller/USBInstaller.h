
// USBInstaller.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
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
// �йش����ʵ�֣������ USBInstaller.cpp
//

class CUSBInstallerApp : public CWinAppEx
{
public:
	CUSBInstallerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
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