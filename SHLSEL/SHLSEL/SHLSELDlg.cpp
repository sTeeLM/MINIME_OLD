// SHLSELDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SHLSEL.h"
#include "SHLSELDlg.h"

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


// CSHLSELDlg 对话框




CSHLSELDlg::CSHLSELDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSHLSELDlg::IDD, pParent)
	, m_bShel2(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSHLSELDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_SHEL1, m_bShel2);
}

BEGIN_MESSAGE_MAP(CSHLSELDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CSHLSELDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CSHLSELDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSHLSELDlg 消息处理程序

BOOL CSHLSELDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	GetDlgItem(IDC_RADIO_SHEL1)->SetWindowText(theApp.m_Shel1.Name);
	GetDlgItem(IDC_RADIO_SHEL2)->SetWindowText(theApp.m_Shel2.Name);

	GetDlgItem(IDC_STATIC_DESC1)->SetWindowText(theApp.m_Shel1.Description);
	GetDlgItem(IDC_STATIC_DESC2)->SetWindowText(theApp.m_Shel2.Description);

	m_hWndMaster = this->m_hWnd;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSHLSELDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSHLSELDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
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

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CSHLSELDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSHLSELDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

HWND CSHLSELDlg::m_hWndMaster = NULL;

DWORD WINAPI CSHLSELDlg::fnMon(LPVOID lpParameter)
{
	SHELConfig * pCfg = (SHELConfig *)lpParameter;
	TCHAR Command[SHEL_MAX_CMD_LEN + SHEL_MAX_ARG_LEN + 1];
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInformation;
	
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	memset(&ProcessInformation, 0, sizeof(ProcessInformation));
	StartupInfo.cb = sizeof(StartupInfo);

	wsprintf(Command, _T("%s %s"), pCfg->Cmd, pCfg->Arg);
	
	if( ! CreateProcess(
		pCfg->Cmd, // LPCTSTR lpApplicationName,
		Command,   // LPTSTR lpCommandLine,
		NULL,      // LPSECURITY_ATTRIBUTES lpProcessAttributes,
		NULL,      // LPSECURITY_ATTRIBUTES lpThreadAttributes,
		FALSE,     // BOOL bInheritHandles,
		0,         // DWORD dwCreationFlags,
		NULL,      // LPVOID lpEnvironment,
		pCfg->WorkDir[0] == 0 ? NULL : pCfg->WorkDir,      // LPCTSTR lpCurrentDirectory,
		&StartupInfo,// LPSTARTUPINFO lpStartupInfo,
		&ProcessInformation// LPPROCESS_INFORMATION lpProcessInformation
	)) return 1;

	::CloseHandle(ProcessInformation.hThread);
	::WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
	::CloseHandle(ProcessInformation.hProcess);

	::ShowWindow(m_hWndMaster, SW_SHOWNORMAL);

	return 0;
}

void CSHLSELDlg::RunProcess(SHELConfig * pCfg)
{
	HANDLE hThread = NULL;
	DWORD dwThreadID;

	// hide windows
	::ShowWindow(m_hWndMaster, SW_HIDE);

	hThread = ::CreateThread(
		NULL, //LPSECURITY_ATTRIBUTES lpThreadAttributes,
		0,	  //SIZE_T dwStackSize,
		fnMon, //LPTHREAD_START_ROUTINE lpStartAddress,
		pCfg, //LPVOID lpParameter,
		0,	  //DWORD dwCreationFlags,
		&dwThreadID // LPDWORD lpThreadId
	);

	if(hThread != NULL)
		::CloseHandle(hThread); 
}

void CSHLSELDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	if(m_bShel2) {
		RunProcess(&theApp.m_Shel2);
	} else {
		RunProcess(&theApp.m_Shel1);
	}
}
