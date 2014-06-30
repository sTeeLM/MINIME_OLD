
// USBInstallerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "USBInstaller.h"
#include "USBInstallerDlg.h"
#include "../libFAT/libFAT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CUSBInstallerDlg 对话框




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

// CUSBInstallerDlg 消息处理程序

BOOL CUSBInstallerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
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
	m_btnStartStop.SetCaption(_T("开始"),_T("取消"));
	m_ctlLog.Initialize();

	ResetBurn();

	m_btnStartStop.EnableWindow(FALSE);

	m_ctlProgress.Reset();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUSBInstallerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUSBInstallerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUSBInstallerDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CUSBInstallerDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
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
				_this->m_ctlLog.Log(CLogBox::LOG_DBG , _T("枚举目录 %s"), strPathNew);
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
				_this->m_ctlLog.Log(CLogBox::LOG_DBG , _T("枚举源目录 %s"), strPathSrc);
				strPathDst = szDst;
				strPathDst += _T("\\");
				strPathDst += data.cFileName;
				_this->m_ctlLog.Log(CLogBox::LOG_DBG , _T("创建目标目录 %s"), strPathDst);
				if(FAT_mkdir(strPathDst) != FATR_OK) {
					_this->m_ctlLog.Log(CLogBox::LOG_DBG , _T("创建目标目录 %s 失败"), strPathDst);
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
				_this->m_ctlLog.Log(CLogBox::LOG_DBG , _T("复制文件 %s -> %s"), strPathSrc, strPathDst);
				if(!CopySingleFile(strPathSrc, strPathDst, _this)) {
					_this->m_ctlLog.Log(CLogBox::LOG_DBG , _T("复制文件 %s -> %s 失败"), strPathSrc, strPathDst);
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
#define MINIME_FREE_SIZE 500 * 1024 * 1024 // part 2 最小空闲空间，part2除了放MINIME，还需要保留这么多空间
#define MINIME_MIN_SIZE 100 * 1024 * 1024  // MINIME 最小大小，如果MINIMI不足这个大小，按照这个大小算
#define CUSTOM_MIN_SIZE	100 * 1024 * 1024  // part 1 最小空间，不然格式化有问题
BOOL CUSBInstallerDlg::DetectPartSize(usb_device_info_t *device, DWORD * szt, CUSBInstallerDlg * _this)
{
	DWORD total_cyl, track_p_cyl, sector_p_track, byte_p_sector, total_sector;
	LONGLONG total_bytes, cyl_bytes, p1_size, p2_size, p1_cyl;
	
	if(FAT_geometry(0, &total_cyl, &track_p_cyl, &sector_p_track, &byte_p_sector, &total_sector) == FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("U盘 %s 参数为:"), device->friendname);
		total_bytes = (LONGLONG)total_sector * 512;
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("总大小 %s (%I64u bytes)"), HumanSize(total_bytes), total_bytes);
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("总扇区数: %u"), total_sector);
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("总柱面数: %u"), total_cyl);
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("磁道/柱面: %u"), track_p_cyl);
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("扇区/磁道: %u"), sector_p_track);
		_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("字节/扇区: %u"), byte_p_sector);
		
		cyl_bytes = (LONGLONG)track_p_cyl * sector_p_track * byte_p_sector;
		p2_size = _this->m_nRootSize > MINIME_MIN_SIZE ? _this->m_nRootSize : MINIME_MIN_SIZE;
		p2_size += MINIME_FREE_SIZE;
		if(p2_size + CUSTOM_MIN_SIZE > total_bytes) {
			_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("U盘大小不够,最小 %I64u bytes"), p2_size + CUSTOM_MIN_SIZE);
		} else {
			p1_cyl = total_cyl - (p2_size /(cyl_bytes));
			p1_size = total_bytes - p2_size;
			szt[0] = (DWORD)p1_cyl;
			szt[1] = (DWORD)(-1);
			szt[2] = (DWORD)0;
			szt[3] = (DWORD)0;
			_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("分区1大小: %I64u bytes, 分区2大小: %I64u bytes"), 
				p1_size, p2_size);
			return TRUE;
		}
	} else {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("无法获得U盘参数"));
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

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("探测源目录 %s"), _this->m_strRoot);
	// 计算源目录大小
	_this->m_nRootSize = _this->EnumRootDir(_this->m_strRoot, _this);
	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("目录 %s 大小 %s (%I64u bytes)"), _this->m_strRoot, 
		HumanSize(_this->m_nRootSize), _this->m_nRootSize);

	_this->m_ctlProgress.SetTotal(_this->m_nRootSize);

	if(_this->m_strSyslinuxDir.GetLength() == 0) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR ,_T("没有发现syslinux 安装目录"));
		goto stop;
	} else {
		_this->m_ctlLog.Log(CLogBox::LOG_INFO ,_T("syslinux 安装目录为 %s"), _this->m_strSyslinuxDir);
	}

	if(_this->m_bCancelBurn)
		goto stop;

	// 打开U盘

	device = OpenCurentUSB(_this, txtBuf);
	if(device == NULL || FAT_attach_disk(device->hdevice, 0) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("无法打开U盘 %s"), txtBuf);
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	// 探测MINIME的分区大小
	if(!DetectPartSize(device, szt, _this)) {
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("为U盘分区 %s"), txtBuf);
	if(FAT_fdisk(0, szt, NULL) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("无法为U盘 %s 分区"), txtBuf);
		goto stop;
	}
	
	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("格式化分区1"));
	if((ret = FAT_mkfs(0,1,0,FAT_FS_AUTO)) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("格式化分区1失败"));
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("格式化分区2"));
	if(FAT_mkfs(0,2,0,FAT_FS_AUTO) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("格式化分区2失败"));
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("装载分区2"));
	if((ret = FAT_mount(&p, 0, 2, 1, 1)) != FATR_OK ) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("装载分区2失败"));
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("Copy 文件到分区2"));
	if(!USBCopyFile(_this->m_strRoot, _T("1:\\"), _this)) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("Copy 文件到分区2失败"));
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("卸载分区2"));
	if(FAT_unmount(p) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("卸载分区2失败"));
		goto stop;
	}	
	p = NULL;

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("安装Syslinux到分区2， 版本 %s"), FAT_syslinux_get_version());
	if(FAT_syslinux_install(0, 2, 0, _this->m_strSyslinuxDir, 1) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("安装Syslinux到分区2失败"));
		goto stop;
	}

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("设置卷标"));
	if((ret = FAT_setvollabel(0, 2, _T("MINIME"), 0)) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("设置卷标失败"));
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("关闭U盘"));
	if(FAT_detach_disk(0) != FATR_OK) {
		_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("关闭U盘失败"));
		goto stop;
	}

	if(_this->m_bCancelBurn)
		goto stop;

	CloseUSB(device, _this);
	device = NULL;
	_this->m_ctlLog.Log(CLogBox::LOG_INFO , _T("完成!"));
	bFinish = TRUE;
stop:
	FAT_detach_disk(0);

	if(_this->m_bCancelBurn) {
		_this->m_ctlLog.Log(CLogBox::LOG_WARN , _T("用户取消"));
	} else {
		if(!bFinish) {
			_this->m_ctlLog.Log(CLogBox::LOG_ERR , _T("失败!"));
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
	bi.lpszTitle        =   _T("请选择MINIME根目录"); 
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

