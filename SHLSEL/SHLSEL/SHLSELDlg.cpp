// SHLSELDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SHLSEL.h"
#include "SHLSELDlg.h"

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


// CSHLSELDlg �Ի���




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


// CSHLSELDlg ��Ϣ�������

BOOL CSHLSELDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	GetDlgItem(IDC_RADIO_SHEL1)->SetWindowText(theApp.m_Shel1.Name);
	GetDlgItem(IDC_RADIO_SHEL2)->SetWindowText(theApp.m_Shel2.Name);

	GetDlgItem(IDC_STATIC_DESC1)->SetWindowText(theApp.m_Shel1.Description);
	GetDlgItem(IDC_STATIC_DESC2)->SetWindowText(theApp.m_Shel2.Description);

	m_hWndMaster = this->m_hWnd;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSHLSELDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CSHLSELDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSHLSELDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	if(m_bShel2) {
		RunProcess(&theApp.m_Shel2);
	} else {
		RunProcess(&theApp.m_Shel1);
	}
}
