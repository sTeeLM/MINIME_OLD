// SHLSEL.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CSHLSELApp:
// 有关此类的实现，请参阅 SHLSEL.cpp
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

// 重写
	public:
	virtual BOOL InitInstance();
	BOOL ParseConfig(LPCTSTR szConfigFile);

// 实现
	SHELConfig m_Shel1;
	SHELConfig m_Shel2;

	DECLARE_MESSAGE_MAP()
};

extern CSHLSELApp theApp;