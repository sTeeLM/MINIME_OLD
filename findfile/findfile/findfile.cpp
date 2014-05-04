// findfile.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "findfile.h"
#include "CMDLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;
CCMDLine cmdLine;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
		nRetCode = 1;
	}
	else
	{
		theApp.ParseCommandLine (cmdLine);
		if(cmdLine.m_szFile.GetLength() ==0 || cmdLine.m_szFilter.GetLength() == 0) {
			nRetCode = 1;
		} else {
			CFileDialog dlgFile(cmdLine.m_bIsOpen,0,cmdLine.m_szFile,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,cmdLine.m_szFilter);
			if(dlgFile.DoModal() == IDOK) {
				_tprintf(_T("%s"), dlgFile.GetPathName());
			} else {
				nRetCode = 1;
			}
		}
	}

	return nRetCode;
}
