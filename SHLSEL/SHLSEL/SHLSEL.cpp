// SHLSEL.cpp : ����Ӧ�ó��������Ϊ��
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


// CSHLSELApp ����

CSHLSELApp::CSHLSELApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	memset(&m_Shel1, 0, sizeof(m_Shel1));
	memset(&m_Shel2, 0, sizeof(m_Shel2));
}


// Ψһ��һ�� CSHLSELApp ����

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

// CSHLSELApp ��ʼ��

BOOL CSHLSELApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	// SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	// ��ȡ����ini
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
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//  �Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
