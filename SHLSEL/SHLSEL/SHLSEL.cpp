// SHLSEL.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "SHLSEL.h"
#include "SHLSELDlg.h"

#include <Psapi.h>
#include <shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSHLSELApp

BEGIN_MESSAGE_MAP(CSHLSELApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSHLSELApp 构造

CSHLSELApp::CSHLSELApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	memset(&m_Shel1, 0, sizeof(m_Shel1));
	memset(&m_Shel2, 0, sizeof(m_Shel2));
}


// 唯一的一个 CSHLSELApp 对象

CSHLSELApp theApp;


BOOL CSHLSELApp::ParseConfig(LPCTSTR szConfigFile)
{
	DWORD dwRet;

	dwRet = ::GetPrivateProfileString(
		_T("SHEL1"),
		_T("Name"),
		NULL,
		m_Shel1.Name,
		sizeof(m_Shel1.Name)/sizeof(TCHAR),
		szConfigFile);
	if(dwRet == 0) return FALSE;

	dwRet = ::GetPrivateProfileString(
		_T("SHEL1"),
		_T("Description"),
		NULL,
		m_Shel1.Description,
		sizeof(m_Shel1.Description)/sizeof(TCHAR),
		szConfigFile);
	if(dwRet == 0) return FALSE;

	dwRet = ::GetPrivateProfileString(
		_T("SHEL1"),
		_T("Cmd"),
		NULL,
		m_Shel1.Cmd,
		sizeof(m_Shel1.Cmd)/sizeof(TCHAR),
		szConfigFile);
	if(dwRet == 0) return FALSE;

	dwRet = ::GetPrivateProfileString(
		_T("SHEL1"),
		_T("Argument"),
		NULL,
		m_Shel1.Arg,
		sizeof(m_Shel1.Arg)/sizeof(TCHAR),
		szConfigFile);
	if(dwRet == 0)
		m_Shel2.Arg[0] = 0;

	dwRet = ::GetPrivateProfileString(
		_T("SHEL1"),
		_T("WorkDir"),
		NULL,
		m_Shel1.WorkDir,
		sizeof(m_Shel1.WorkDir)/sizeof(TCHAR),
		szConfigFile);
	if(dwRet == 0)
		m_Shel2.WorkDir[0] = 0;

	dwRet = ::GetPrivateProfileString(
		_T("SHEL2"),
		_T("Name"),
		NULL,
		m_Shel2.Name,
		sizeof(m_Shel2.Name)/sizeof(TCHAR),
		szConfigFile);
	if(dwRet == 0) return FALSE;

	dwRet = ::GetPrivateProfileString(
		_T("SHEL2"),
		_T("Description"),
		NULL,
		m_Shel2.Description,
		sizeof(m_Shel2.Description)/sizeof(TCHAR),
		szConfigFile);
	if(dwRet == 0) return FALSE;

	dwRet = ::GetPrivateProfileString(
		_T("SHEL2"),
		_T("Cmd"),
		NULL,
		m_Shel2.Cmd,
		sizeof(m_Shel2.Cmd)/sizeof(TCHAR),
		szConfigFile);
	if(dwRet == 0) return FALSE;

	dwRet = ::GetPrivateProfileString(
		_T("SHEL2"),
		_T("Argument"),
		NULL,
		m_Shel2.Arg,
		sizeof(m_Shel2.Arg)/sizeof(TCHAR),
		szConfigFile);
	if(dwRet == 0)
		m_Shel2.Arg[0] = 0;

	dwRet = ::GetPrivateProfileString(
		_T("SHEL2"),
		_T("WorkDir"),
		NULL,
		m_Shel2.WorkDir,
		sizeof(m_Shel2.WorkDir)/sizeof(TCHAR),
		szConfigFile);
	if(dwRet == 0)
		m_Shel2.WorkDir[0] = 0;

	return TRUE;
}

// CSHLSELApp 初始化

BOOL CSHLSELApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	// SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	// 读取本地ini
	TCHAR Buffer[4096];
	DWORD dwLen = 0;
	HANDLE hProcess = ::GetCurrentProcess();
	dwLen = ::GetModuleFileNameEx(hProcess, NULL, Buffer, sizeof(Buffer)/sizeof(TCHAR));
	if(dwLen == 0)
		return FALSE;
	Buffer[dwLen - 1] = 0;

	if(!::PathRemoveFileSpec(Buffer))
		return FALSE;

	if(!::PathAppend(Buffer, _T("SHLSEL.ini")))
		return FALSE;

	if(!ParseConfig(Buffer)) 
		return FALSE;

	CSHLSELDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此处放置处理何时用“确定”来关闭
		//  对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//  对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
