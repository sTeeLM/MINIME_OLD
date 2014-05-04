// findfile.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "findfile.h"
#include "CMDLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;
CCMDLine cmdLine;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
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
