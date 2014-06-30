
// USBInstallerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "USBInstaller.h"
#include "USBInstallerDlg.h"
#include "../libFAT/libFAT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CUSBInstallerDlg �Ի���




CUSBInstallerDlg::CUSBInstallerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBInstallerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strRoot = _T("");
	m_strSyslinuxDir = _T("");
	m_bCancelBurn = FALSE;
	m_hBurnUSBThread = NULL;
	m_bWaitForClose = FALSE;
	m_nRootSize = 0;
}

void CUSBInstallerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICE, m_ctlDevice);
	DDX_Control(pDX, IDC_STATIC_MINIME_PATH, m_ctlRootPath);
	DDX_Control(pDX, IDC_BUTTON_SEL_ROOT, m_btnSelRoot);
	DDX_Control(pDX, IDC_LIST_LOG, m_ctlLog);
	DDX_Control(pDX, IDC_BUTTON_START_STOP, m_btnStartStop);
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
}
#define WM_BURN_USB_STOP (WM_USER + 1)
#define WM_UPDATE_BURN_USB (WM_USER + 2)

BEGIN_MESSAGE_MAP(CUSBInstallerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_BURN_USB_STOP, OnBurnUSBStop)
	ON_MESSAGE(WM_UPDATE_BURN_USB, OnUpdateBurnUSB)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CUSBInstallerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CUSBInstallerDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE, &CUSBInstallerDlg::OnCbnSelchangeComboDevice)
	ON_BN_CLICKED(IDC_BUTTON_SEL_ROOT, &CUSBInstallerDlg::OnBnClickedButtonSelRoot)
	ON_BN_CLICKED(IDC_BUTTON_START_STOP, &CUSBInstallerDlg::OnBnClickedButtonStartStop)
END_MESSAGE_MAP()


CString CUSBInstallerDlg::HumanSize(LONGLONG Size)
{
	CString ret;
	double p;
	if(Size > 1024 * 1024 * 1024) {
		p = (double)(Size / (double)(1024 * 1024 * 1024));
		ret.Format(_T("%0.2f GB"), p);
	} else if(Size > 1024 * 1024) {
		p = (double)(Size / (double)(1024 * 1024));
		ret.Format(_T("%0.2f MB"), p);
	} else if(Size > 1024 * 1024) {
		p = (double)(Size / (double)(1024));
		ret.Format(_T("%0.2f KB"), p);
	} else {
		p = (double)(Size / (double)(1024));
		ret.Format(_T("%0.2f B"), p);
	}
	return ret;
}

// CUSBInstallerDlg ��Ϣ�������

BOOL CUSBInstallerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	// init combo ctl
	for(int i = 0 ; i < theApp.m_UsbCount; i ++) {
		CString Buffer;
		Buffer.Format(_T("%c: %s %s"),theApp.m_UsbList[i].volume, 
			theApp.m_UsbList[i].friendname, HumanSize(theApp.m_UsbList[i].geometry.DiskSize.QuadPart));
		m_ctlDevice.AddString(Buffer);
	}

	for(int i = 0 ; i < theApp.m_UsbCount; i ++) {
		m_ctlDevice.SetItemData(i, (DWORD_PTR)&theApp.m_UsbList[i]);
	}

	m_ctlDevice.SetCurSel(0);
	
	m_ctlRootPath.SetWindowText(_T(""));
	m_btnStartStop.SetCaption(_T("��ʼ"),_T("ȡ��"));
	m_ctlLog.Initialize();

	ResetBurn();

	m_btnStartStop.EnableWindow(FALSE);

	m_ctlProgress.Reset();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CUSBInstallerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUSBInstallerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CUSBInstallerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUSBInstallerDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//OnOK();
}

void CUSBInstallerDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

void CUSBInstallerDlg::OnCancel()
{
	TRACE(_T("OnCancel called\n"));
	CDialog::OnCancel();
}

void CUSBInstallerDlg::OnCbnSelchangeComboDevice()
{

}

void CUSBInstallerDlg::GetSyslinuxPath(LPCTSTR szPath, CUSBInstallerDlg * _this)
{
	LPTSTR  root = _tcsdup(_this->m_strRoot);
	LPTSTR  sys  = _tcsdup(szPath);
	LPTSTR  p = NULL;
	size_t len;
	if(NULL != root && NULL != sys) {
		p = root;
		len = _tcslen(p);
		if(p[len - 1] == _T('\\'))
			len --;
		p = sys;
		p += len;
		_this->m_strSyslinuxDir = p;
	}
	if(NULL != root)
		free(root);
	if(NULL != sys)
		free(sys);
}

LONGLONG CUSBInstallerDlg::EnumRootDir(CString & strPath, CUSBInstallerDlg * _this)
{
    WIN32_FIND_DATA data;
    HANDLE sh = NULL;
	CString strPathNew = strPath;
	strPathNew += _T("\\*");
	LONGLONG Size = 0;

    sh = FindFirstFile(strPathNew, &data);

	if(_this->m_bCancelBurn) {
		goto err;
	}

    if (sh == INVALID_HANDLE_VALUE ) {
		goto err;
    }

    do
    {
        // skip current and parent
		if(_tcscmp(data.cFileName, _T("..")) != 0 && _tcscmp(data.cFileName, _T(".")) != 0)
        {

            // if found object is ...
            if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
            {
                // directory, then search it recursievly
				strPathNew = strPath + _T("\\");
				strPathNew += data.cFileName;
				_this->m_ctlLog.Log(CLogBox::LOG_DBG , _T("ö��Ŀ¼ %s"), strPathNew);
                Size += EnumRootDir(strPathNew, _this);
				if(!_tcsicmp(data.cFileName, _T("syslinux"))) {
					GetSyslinuxPath(strPathNew, _this);
				}
            } else
            {
                // otherwise get object size and add it to directory size
				//::PostMessage(_this->m_hWnd, WM_UPDATE_BURN, data.nFileSizeHigh, data.nFileSizeLow);
                Size += (data.nFileSizeHigh * (MAXDWORD ) + data.nFileSizeLow);
            }
        }

    } while (FindNextFile(sh, &data) && !_this->m_bCancelBurn); // do

err:
	if(NULL != sh) {
		FindClose(sh);
		sh = NULL;
	}

	return Size;

} 

void CUSBInstallerDlg::CloseUSB(usb_device_info_t * device, CUSBInstallerDlg * _this)
{
	if(NULL != device) {
		if(device->hdevice != INVALID_HANDLE_VALUE) {
			::CloseHandle(device->hdevice);
			device->hdevice  = INVALID_HANDLE_VALUE;
		}
		if(device->hvolume != INVALID_HANDLE_VALUE) {
			::CloseHandle(device->hvolume);
			device->hvolume  = INVALID_HANDLE_VALUE;
		}
	}
}

usb_device_info_t * CUSBInstallerDlg::OpenCurentUSB(CUSBInstallerDlg * _this, CString & strName)
{
	int n = _this->m_ctlDevice.GetCurSel();
	TCHAR driveName[128];
	TCHAR volumeName[128];
	DWORD count, dwErr;
	usb_device_info_t * p = NULL;

	if(n < 0) {
		return NULL;
	}

	p = (usb_device_info_t *)_this->m_ctlDevice.GetItemData(n);
	
	if(NULL == p)
		return NULL;

	strName = p->friendname;

	// open volume
	_stprintf_s(volumeName, _countof(driveName), _T("\\\\.\\%c:"), p->volume);
	p->hvolume = CreateFile(volumeName,
				   GENERIC_READ| GENERIC_WRITE,
				   FILE_SHARE_READ|FILE_SHARE_WRITE,
				   NULL, OPEN_EXISTING, 0, NULL);
	if(INVALID_HANDLE_VALUE == p->hvolume) {
		dwErr = ::GetLastError();
		goto err;
	}

	// unmount volume
	if(!DeviceIoControl(p->hvolume, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &count, NULL)) {
		dwErr = ::GetLastError();
		goto err;
	}
	// lock volume
	if(!DeviceIoControl(p->hvolume, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &count, NULL)) {
		dwErr = ::GetLastError();
		goto err;
	}
	// open device
	_stprintf_s(driveName, _countof(driveName), _T("\\\\.\\PHYSICALDRIVE%d"), p->device_num);
	p->hdevice = CreateFile(driveName,
				   GENERIC_READ| GENERIC_WRITE,
				   FILE_SHARE_READ|FILE_SHARE_WRITE,
				   NULL, OPEN_EXISTING, 0, NULL);

	return p;
err:
	if(p->hdevice != INVALID_HANDLE_VALUE) {
		::CloseHandle(p->hdevice);
		p->hdevice = INVALID_HANDLE_VALUE;
	}
	if(p->hvolume != INVALID_HANDLE_VALUE) {
		::CloseHandle(p->hvolume);
		p->hvolume = INVALID_HANDLE_VALUE;
	}
	return NULL;
}

BOOL CUSBInstallerDlg::CopySingleFile(LPCTSTR szSrc, LPCTSTR szDst, CUSBInstallerDlg * _this)
{
	BYTE Buffer[40960];
	LARGE_INTEGER size;
	BOOL ret = FALSE;
	DWORD br, brs;
	FAT_FP fp = NULL;
	HANDLE file = INVALID_HANDLE_VALUE;

	if(FAT_open(&fp, szDst, FAT_MODE_OPEN_ALWAYS | FAT_MODE_WRITE) != FATR_OK) {
		goto err;
	}

	if(FAT_truncate(fp) != FATR_OK) {
		goto err;
	}

	file = ::CreateFile(
		szSrc,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if(file == INVALID_HANDLE_VALUE) {
		goto err;
	}
	
	if(!::GetFileSizeEx(file, &size)) {
		goto err;
	}

	while(size.QuadPart && !_this->m_bCancelBurn) {
		br = sizeof(Buffer);
		if(!::ReadFile(file, Buffer, sizeof(Buffer),&br,NULL)) {
			goto err;
		}
		if(0 == br) {
			break;
		}
		brs = br;
		if(FAT_write(fp, Buffer, br, (UINT*)&br) != FATR_OK || br != brs) {
			goto err;
		}
		size.QuadPart -= br;
		_this->m_ctlProgress.UpdateProgress(br);
	}
	if(!_this->m_bCancelBurn)
		ret = TRUE;
err:
	if(NULL != fp) {
		FAT_close(fp);
		fp = NULL;
	}
	if(NULL != file) {
		::CloseHandle(file);
		file = INVALID_HANDLE_VALUE;
	}
	return ret;
}

BOOL CUSBInstallerDlg::USBCopyFile(LPCTSTR szSrc, LPCTSTR szDst, CUSBInstallerDlg * _this)
{
    WIN32_FIND_DATA data;
    HANDLE sh = NULL;
	BOOL ret = FALSE;
	CString strPathSrc = szSrc;
	CString strPathDst = szDst;
	strPathSrc += _T("\\*");


    sh = FindFirstFile(strPathSrc, &data);

	if(_this->m_bCancelBurn) {
		goto err;
	}

    if (sh == INVALID_HANDLE_VALUE ) {
		goto err;
    }

    do
    {
        // skip current and parent
		if(_tcscmp(data.cFileName, _T("..")) != 0 && _tcscmp(data.cFileName, _T(".")) != 0)
        {

            // if found object is ...
            if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
            {
                // directory, then search it recursievly
				strPathSrc = szSrc;
				strPathSrc = strPathSrc + _T("\\");
				strPathSrc += data.cFileName;
				_this->m_ctlLog.Log(CLogBox::LOG_DBG , _T("ö��ԴĿ¼ %s"), strPathSrc);
				strPathDst = szDst;
				strPathDst += _T("\\");
				strPathDst += data.cFileName;
				_this->m_ctlLog.Log(CLogBox::LOG_DBG , _T("����Ŀ��Ŀ¼ %s"), strPathDst);
				if(FAT_mkdir(strPathDst) != FATR_OK) {
					_this->m_ctlLog.Log(CLogBox::LOG_DBG , _T("����Ŀ��Ŀ¼ %s ʧ��"), strPathDst);
					goto err;
				}
				if( !USBCopyFile(strPathSrc, strPathDst,_this)) {
					goto err;
				}

            } else
            {
                // otherwise get object size and add it to directory size
				//::PostMessage(_this->m_hWnd, WM_UPDATE_BURN, data.nFileSizeHigh, data.nFileSizeLow);
                //Size += (data.nFileSizeHigh * (MAXDWORD ) + data.nFileSizeLow);
				strPathSrc = szSrc;
				strPathSrc = strPathSrc + _T("\\");
				strPathSrc += data.cFileName;
				strPathDst = szDst;
				strPathDst += _T("\\");
				strPathDst += data.cFileName;
				_this->m_ctlLog.Log(CLogBox::LOG_DBG , _T("�����ļ� %s -> %s"), strPathSrc, strPathDst);
				if(!CopySingleFile(strPathSrc, strPathDst, _this)) {
					_this->m_ctlLog.Log(CLogBox::LOG_DBG , _T("�����ļ� %s -> %s ʧ��"), strPathSrc, strPathDst);
					goto err;
				}
            }
        }

    } while (FindNextFile(sh, &data) && !_this->m_bCancelBurn); // do

	ret = TRUE;
err:
	if(NULL != sh) {
		FindClose(sh);
		sh = NULL;
	}

	return ret;

}
#define MINIME_FREE_SIZE 500 * 1024 * 1024 // part 2 ��С���пռ䣬part2���˷�MINIME������Ҫ������ô��ռ�
#define MINIME_MIN_SIZE 100 * 1024 * 1024  // MINIME ��С��С�����MINIMI���������С�����������С��
#define CUSTOM_MIN_SIZE	100 * 1024 * 1024  // part 1 ��С�ռ䣬��Ȼ��ʽ��������
BOOL CUSBInstallerDlg::DetectPartSize(usb_device_info_t *device, DWORD * szt, CUSBInstallerDlg * _this)
{
	DWORD total_cyl, track_p_cyl, sector_p_track, byte_p_sector, total_sector;
	LONGLONG total_bytes, cyl_bytes, p1_size, p2_size, p1_cyl;
	
	if(FAT_geometry(0, &total_cyl, &track_p_cyl, &sector_p_track, &byte_p_sector, &total_sector) == FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("U�� %s ����Ϊ:"), device->friendname);
		total_bytes = (LONGLONG)total_sector * 512;
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("�ܴ�С %s (%I64u bytes)"), HumanSize(total_bytes), total_bytes);
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("��������: %u"), total_sector);
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("��������: %u"), total_cyl);
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("�ŵ�/����: %u"), track_p_cyl);
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("����/�ŵ�: %u"), sector_p_track);
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("�ֽ�/����: %u"), byte_p_sector);
		
		cyl_bytes = (LONGLONG)track_p_cyl * sector_p_track * byte_p_sector;
		p2_size = _this->m_nRootSize > MINIME_MIN_SIZE ? _this->m_nRootSize : MINIME_MIN_SIZE;
		p2_size += MINIME_FREE_SIZE;
		if(p2_size + CUSTOM_MIN_SIZE > total_bytes) {
			_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("U�̴�С����,��С %I64u bytes"), p2_size + CUSTOM_MIN_SIZE);
		} else {
			p1_cyl = total_cyl - (p2_size /(cyl_bytes));
			p1_size = total_bytes - p2_size;
			szt[0] = (DWORD)p1_cyl;
			szt[1] = (DWORD)(-1);
			szt[2] = (DWORD)0;
			szt[3] = (DWORD)0;
			_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("����1��С: %I64u bytes, ����2��С: %I64u bytes"), 
				p1_size, p2_size);
			return TRUE;
		}
	} else {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("�޷����U�̲���"));
	}
	return FALSE;
}

DWORD WINAPI CUSBInstallerDlg::fnBurnUSB(LPVOID lpParameter)
{
	FAT_FSP p = NULL;
	CString txtBuf;
	CUSBInstallerDlg * _this = (CUSBInstallerDlg *)lpParameter;
	usb_device_info_t * device = NULL;
	DWORD szt[4];
	FAT_RESULT ret;
	BOOL bFinish = FALSE;

	TRACE(_T("fnBurnUSB start\n"));

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("̽��ԴĿ¼ %s"), _this->m_strRoot);
	// ����ԴĿ¼��С
	_this->m_nRootSize = _this->EnumRootDir(_this->m_strRoot, _this);
	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("Ŀ¼ %s ��С %s (%I64u bytes)"), _this->m_strRoot, 
		HumanSize(_this->m_nRootSize), _this->m_nRootSize);

	_this->m_ctlProgress.SetTotal(_this->m_nRootSize);

	if(_this->m_strSyslinuxDir.GetLength() == 0) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR ,_T("û�з���syslinux ��װĿ¼"));
		goto stop;
	} else {
		_this->m_ctlLog.Log(CLogBox::LOG_INFO ,_T("syslinux ��װĿ¼Ϊ %s"), _this->m_strSyslinuxDir);
	}

	if(_this->m_bCancelBurn)
		goto stop;

	// ��U��

	device = OpenCurentUSB(_this, txtBuf);
	if(device == NULL || FAT_attach_disk(device->hdevice, 0) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("�޷���U�� %s"), txtBuf);
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	// ̽��MINIME�ķ�����С
	if(!DetectPartSize(device, szt, _this)) {
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("ΪU�̷��� %s"), txtBuf);
	if(FAT_fdisk(0, szt, NULL) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("�޷�ΪU�� %s ����"), txtBuf);
		goto stop;
	}
	
	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("��ʽ������1"));
	if((ret = FAT_mkfs(0,1,0,FAT_FS_AUTO)) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("��ʽ������1ʧ��"));
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("��ʽ������2"));
	if(FAT_mkfs(0,2,0,FAT_FS_AUTO) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("��ʽ������2ʧ��"));
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("װ�ط���2"));
	if((ret = FAT_mount(&p, 0, 2, 1, 1)) != FATR_OK ) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("װ�ط���2ʧ��"));
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("Copy �ļ�������2"));
	if(!USBCopyFile(_this->m_strRoot, _T("1:\\"), _this)) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("Copy �ļ�������2ʧ��"));
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("ж�ط���2"));
	if(FAT_unmount(p) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("ж�ط���2ʧ��"));
		goto stop;
	}	
	p = NULL;

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("��װSyslinux������2�� �汾 %s"), FAT_syslinux_get_version());
	if(FAT_syslinux_install(0, 2, 0, _this->m_strSyslinuxDir, 1) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("��װSyslinux������2ʧ��"));
		goto stop;
	}

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("���þ��"));
	if((ret = FAT_setvollabel(0, 2, _T("MINIME"), 0)) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("���þ��ʧ��"));
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("�ر�U��"));
	if(FAT_detach_disk(0) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("�ر�U��ʧ��"));
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	CloseUSB(device, _this);
	device = NULL;
	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("���!"));
	bFinish = TRUE;
stop:
	FAT_detach_disk(0);

	if(_this->m_bCancelBurn) {
		_this->m_ctlLog.Log(CLogBox::LOG_WARN , _T("�û�ȡ��"));
	} else {
		if(!bFinish) {
			_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("ʧ��!"));
		}
	}
	if(NULL != p) {
		FAT_unmount(p);
		p = NULL;
	}
	if(device != NULL) {
		CloseUSB(device, _this);
		device = NULL;
	}
	TRACE(_T("fnBurnUSB stop\n"));
	::SendMessage(_this->m_hWnd, WM_BURN_USB_STOP, 0, 0);
	return 0;
}

LRESULT CUSBInstallerDlg::OnUpdateBurnUSB(WPARAM wp, LPARAM lp)
{
	/*
	m_nRootSize += (wp * (MAXDWORD ) + lp);
	if(m_nRootSizeUpdateCnt++ > 1024) {
		m_nRootSizeUpdateCnt = 0;
		m_ctlRootSize.SetWindowText(HumanSize(m_nRootSize));
	}
	*/
	return 0;
}

void CUSBInstallerDlg::ResetBurn()
{
	m_btnStartStop.Switch(0);
	m_nRootSize = 0;
	m_strSyslinuxDir = _T("");
	m_ctlRootPath.SetWindowText(m_strRoot);
	m_btnStartStop.EnableWindow(m_strRoot.GetLength() != 0);
	m_ctlProgress.Reset();
}

LRESULT CUSBInstallerDlg::OnBurnUSBStop(WPARAM wp, LPARAM lp)
{
	TRACE(_T("OnBurnUSBStop %d, %d\n"), wp, lp);

	m_bCancelBurn = FALSE;
	if(NULL != m_hBurnUSBThread) {
		::CloseHandle(m_hBurnUSBThread);
		m_hBurnUSBThread = NULL;
	}

	m_btnStartStop.Switch();
	m_btnSelRoot.EnableWindow();
	m_ctlDevice.EnableWindow();

	ResetBurn();

	if(m_bWaitForClose) {
		TRACE(_T("Close Windows in OnBurnUSBStop"));
		OnClose();
	}
	
	return 0;
}

void CUSBInstallerDlg::StartBurnUSB()
{
	DWORD dwThreadID;
	m_hBurnUSBThread = ::CreateThread(NULL, 0, fnBurnUSB, this, 0, &dwThreadID);
	if(NULL == m_hBurnUSBThread) {
		m_bCancelBurn = FALSE;
	} else {
		m_btnSelRoot.EnableWindow(FALSE);
		m_ctlDevice.EnableWindow(FALSE);
	}

}

void CUSBInstallerDlg::OnBnClickedButtonSelRoot()
{
	BROWSEINFO   bi; 
	int nRetCode;
	ZeroMemory(&bi,   sizeof(bi)); 
	TCHAR   szDisplayName[MAX_PATH]; 
	szDisplayName[0]    =   0;  

	bi.hwndOwner        =   NULL; 
	bi.pidlRoot         =   NULL; 
	bi.pszDisplayName   =   szDisplayName; 
	bi.lpszTitle        =   _T("��ѡ��MINIME��Ŀ¼"); 
	bi.ulFlags          =   BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE|BIF_NONEWFOLDERBUTTON;
	bi.lParam           =   NULL; 
	bi.iImage           =   0;  

	LPITEMIDLIST   pidl   =   SHBrowseForFolder(&bi);
	TCHAR   szPathName[MAX_PATH]; 
	if   (NULL   !=   pidl)
	{
		 BOOL bRet = SHGetPathFromIDList(pidl,szPathName);
		 if(FALSE == bRet) {
			  nRetCode = 1;
		 } else {
			 nRetCode = 0;
			_tprintf(_T("%s"), szPathName);
		 }
	} else {
		nRetCode = 1;
	}

	if(nRetCode == 0) {
		m_strRoot = szPathName;
		m_nRootSize = 0;
		m_ctlRootPath.SetWindowText(m_strRoot);
		m_btnStartStop.EnableWindow(TRUE);
	} else {
		m_btnStartStop.EnableWindow(FALSE);
	}
}

void CUSBInstallerDlg::OnBnClickedButtonStartStop()
{
	if(m_btnStartStop.GetSwitch() == 0) { // start
		m_bCancelBurn = FALSE;
		m_btnStartStop.Switch();
		StartBurnUSB();
	} else { // stop
		m_bCancelBurn = TRUE;
	}
}

void CUSBInstallerDlg::OnClose()
{
	TRACE(_T("OnClose called\n"));
	m_bWaitForClose = TRUE;
	
	if(m_hBurnUSBThread != NULL) {
		m_bCancelBurn = TRUE;
	} else {
		CDialog::OnClose();
	}

	CDialog::OnClose();
}

