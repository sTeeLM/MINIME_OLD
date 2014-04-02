// SHLSEL.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSHLSELApp:
// �йش����ʵ�֣������ SHLSEL.cpp
//

#define SHEL_MAX_NAME_LEN 255
#define SHEL_MAX_DESC_LEN 1023
#define SHEL_MAX_CMD_LEN 1023
#define SHEL_MAX_ARG_LEN 1023
#define SHEL_MAX_WORKDIR_LEN 1023

struct SHELConfig
{
	TCHAR Name[SHEL_MAX_NAME_LEN + 1];
	TCHAR Description[SHEL_MAX_DESC_LEN + 1];
	TCHAR Cmd[SHEL_MAX_CMD_LEN + 1];
	TCHAR Arg[SHEL_MAX_ARG_LEN + 1];
	TCHAR WorkDir[SHEL_MAX_WORKDIR_LEN];
};

class CSHLSELApp : public CWinApp
{
public:
	CSHLSELApp();

// ��д
	public:
	virtual BOOL InitInstance();
	BOOL ParseConfig(LPCTSTR szConfigFile);

// ʵ��
	SHELConfig m_Shel1;
	SHELConfig m_Shel2;

	DECLARE_MESSAGE_MAP()
};

extern CSHLSELApp theApp;