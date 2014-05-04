#pragma once
#include "afxwin.h"

class CCMDLine :
	public CCommandLineInfo
{
public:
	CCMDLine(void);
	~CCMDLine(void);
	virtual void ParseParam( 
	   const TCHAR* pszParam,  
	   BOOL bFlag, 
	   BOOL bLast
	);

	CString m_szFile;
	CString m_szFilter;
	BOOL   m_bIsOpen;
};
