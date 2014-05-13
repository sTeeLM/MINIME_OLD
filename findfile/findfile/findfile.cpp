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
		if(cmdLine.m_bIsFileDlg) {
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
		} else {
			BROWSEINFO   bi; 
			ZeroMemory(&bi,   sizeof(bi)); 
			TCHAR   szDisplayName[MAX_PATH]; 
			szDisplayName[0]    =   0;  

			bi.hwndOwner        =   NULL; 
			bi.pidlRoot         =   NULL; 
			bi.pszDisplayName   =   szDisplayName; 
			bi.lpszTitle        =   _T("��ѡ���ļ���"); 
			bi.ulFlags          =   BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
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
			}
		}
	}

	return nRetCode;
}
