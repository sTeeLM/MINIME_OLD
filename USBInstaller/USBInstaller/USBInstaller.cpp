
// USBInstaller.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "USBInstaller.h"
#include "USBInstallerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUSBInstallerApp

BEGIN_MESSAGE_MAP(CUSBInstallerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUSBInstallerApp ����

CUSBInstallerApp::CUSBInstallerApp()
{
	memset(m_UsbList,0,sizeof(m_UsbList));
	m_UsbCount = 0;
	for(int i = 0 ; i < _countof(m_UsbList) ; i++) {
		m_UsbList[i].hdevice = m_UsbList[i].hvolume = INVALID_HANDLE_VALUE;
	}
}


// Ψһ��һ�� CUSBInstallerApp ����

CUSBInstallerApp theApp;

int CUSBInstallerApp::GetUsbDeviceList(usb_device_info *usb_list, int list_size)
{
	int usb_device_cnt = 0;

    TCHAR disk_path[5] = {0}; 
    TCHAR device_path[10] = {0};        
	DWORD all_disk = GetLogicalDrives();
		
	int i = 0;
	DWORD bytes_returned = 0;
	STORAGE_DEVICE_NUMBER device_num;
	while (all_disk && usb_device_cnt < list_size)
	{
		if ((all_disk & 0x1) == 1)             
		{       
			_stprintf_s (disk_path,_countof(disk_path), _T("%c:"), _T('A')+i);
			_stprintf_s(device_path,_countof(device_path), _T("\\\\.\\%s"), disk_path);
			if (GetDriveType(disk_path) == DRIVE_REMOVABLE)                 
			{       
				// get this usb device id
				HANDLE hDevice = CreateFile(device_path, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
				if (DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, 
								    NULL, 0, 
									&device_num, sizeof(device_num), 
									&bytes_returned, (LPOVERLAPPED) NULL))
				{
					usb_list[usb_device_cnt].volume = _T('A') + i;
					usb_list[usb_device_cnt].device_num = device_num.DeviceNumber;
					usb_device_cnt++;
				}
				CloseHandle(hDevice);
				hDevice = 0;
			}			
		}
		all_disk = all_disk >> 1;
		i++;
	}
	
	return usb_device_cnt;
}

int CUSBInstallerApp::GetUsbDeviceGeometry(usb_device_info *usb_list, int list_size)
{
	int i;
	int res = 0;
	TCHAR driveName[128];
	HANDLE drive;
	DWORD count;
	for(i = 0 ; i < list_size; i ++) {
		 _stprintf_s(driveName, _countof(driveName), _T("\\\\.\\PHYSICALDRIVE%d"), usb_list[i].device_num);

		drive = CreateFile(driveName,
				   GENERIC_READ| GENERIC_WRITE,
				   FILE_SHARE_READ|FILE_SHARE_WRITE,
				   NULL, OPEN_EXISTING, 0, NULL);

		if (drive == INVALID_HANDLE_VALUE) {
			TRACE(_T("Accessing physical drive Error"));
			goto err;
		}
		if (!DeviceIoControl(drive, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, NULL, 0,
				&usb_list[i].geometry, sizeof(usb_list[i].geometry), &count, NULL)) {
			CloseHandle(drive);
			goto err;
		}
	}
	return list_size;
err:
	return 0;
}

int CUSBInstallerApp::GetUsbDeviceFriendName(usb_device_info *usb_list, int list_size)
{	
	int i = 0;
	int res = 0;
	int ret = 0;
	HDEVINFO hDevInfo;  
	SP_DEVINFO_DATA DeviceInfoData = {sizeof(DeviceInfoData)};   

	// get device class information handle
	hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK,0, 0, DIGCF_PRESENT|DIGCF_DEVICEINTERFACE);       
	if (hDevInfo == INVALID_HANDLE_VALUE)     
	{         
		res = GetLastError();     
		return ret;
	}  

	// enumerute device information
	DWORD required_size = 0;
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
	{		
		DWORD DataT;         
		TCHAR friendly_name[2046] = {0};         
		DWORD buffersize = 2046;        
		DWORD req_bufsize = 2046;      

		// get device friendly name
		if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, &DataT, (LPBYTE)friendly_name, buffersize, &req_bufsize))
		{
			continue;
		}
		if (_tcsstr (friendly_name, _T("USB")) == 0)
		{
			continue;
		}

		int index = 0;
		SP_DEVICE_INTERFACE_DATA did = {sizeof(did)};
		PSP_DEVICE_INTERFACE_DETAIL_DATA pdd = NULL;

		while(1)
		{
			// get device interface data
			if (!SetupDiEnumDeviceInterfaces(hDevInfo, &DeviceInfoData, &GUID_DEVINTERFACE_DISK, index++, &did))
			{
				res = GetLastError();
				if( ERROR_NO_MORE_DEVICES == res || ERROR_NO_MORE_ITEMS == res)
					break;
			}

			// get device interface detail size
			if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &did, NULL, 0, &required_size, NULL))
			{
				res = GetLastError();
				if(ERROR_INSUFFICIENT_BUFFER == res)
				{
					pdd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, required_size);
					pdd->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
				}
				else
					break;
			}

			// get device interface detail
			if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &did, pdd, required_size, NULL, NULL))
			{
				res = GetLastError();
				LocalFree(pdd);
				pdd = NULL;
				break;
			}
			
			// get device number
			DWORD bytes_returned = 0;
			STORAGE_DEVICE_NUMBER device_num;
			HANDLE hDevice = CreateFile(pdd->DevicePath, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if (DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, 
								NULL, 0, 
								&device_num, sizeof(device_num), 
								&bytes_returned, (LPOVERLAPPED) NULL))
			{
				for (int usb_index = 0; usb_index < list_size; usb_index++)
				{
					if (device_num.DeviceNumber == usb_list[usb_index].device_num)
					{
						_tcscpy_s (usb_list[usb_index].friendname,sizeof(usb_list[usb_index].friendname)/sizeof(TCHAR)
							, friendly_name);
						ret ++;
						break;			
					}
				}
			}
			CloseHandle(hDevice);
			LocalFree(pdd);
			pdd = NULL;
		}
	}
	
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return ret;
}

BOOL CUSBInstallerApp::InitializeDevice()
{
	memset(m_UsbList, 0, sizeof(m_UsbList));  
	int usb_cnt = GetUsbDeviceList(m_UsbList, _countof(m_UsbList));
	int usb_total = usb_cnt;
	TRACE(_T("System has %d USB disk.\n"), usb_cnt);  
    if (usb_cnt > 0)  
    {  
        usb_cnt = GetUsbDeviceFriendName(m_UsbList, usb_cnt);
		if(usb_cnt == usb_total) {
			usb_cnt = GetUsbDeviceGeometry(m_UsbList, usb_cnt);
		}
    }
 
    for (int i = 0; i < usb_cnt; i++)  
	{  
		TRACE(_T("%c: %s\n"), m_UsbList[i].volume, m_UsbList[i].friendname);  
	}  

	if(usb_total == usb_cnt) {
		m_UsbCount = usb_cnt;
	}

	return m_UsbCount != 0;
}

// CUSBInstallerApp ��ʼ��

BOOL CUSBInstallerApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	// SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	if(!InitializeDevice()) {
		AfxMessageBox(_T("�����ҵ��κ�U�̣��˳���"));
		return FALSE;
	}

	if(FAT_initilize() != FATR_OK) {
		AfxMessageBox(_T("���ܳ�ʼ��FAT�⣬�˳���"));
		return FALSE;
	}

	CUSBInstallerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	theApp.ExitInstance();
	return FALSE;
}

int CUSBInstallerApp::ExitInstance()
{
	CWinApp::ExitInstance();
	return 0;
}
