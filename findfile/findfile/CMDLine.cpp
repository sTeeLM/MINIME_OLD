#include "StdAfx.h"
#include "CMDLine.h"

CCMDLine::CCMDLine(void)
{
	m_szFile = _T("");
	m_szFilter = _T("");
	m_bIsOpen  = FALSE;
	m_bIsFileDlg = TRUE;
}

CCMDLine::~CCMDLine(void)
{
}

void CCMDLine::ParseParam( 
	   const TCHAR* pszParam,  
	   BOOL bFlag, 
	   BOOL bLast
	)
{
	TRACE(_T("%s %d %d\n"), pszParam, bFlag, bLast);
	CString item = pszParam;
	CString key, val;
	INT nIndex = 0;
	nIndex = item.Find(_T(":"));
	if(nIndex > 0 && item.GetLength() - nIndex - 1 > 0) {
		key = item.Left(nIndex);
		val = item.Right(item.GetLength() - nIndex - 1);
	} else {
		return;
	}



	if(key.CompareNoCase(_T("file")) == 0) {
		m_szFile = val;
	} else if(key.CompareNoCase(_T("filter")) == 0) {
		m_szFilter = val;
	} else if(key.CompareNoCase(_T("dlgtype")) == 0) {
		if(val.CompareNoCase(_T("open")) == 0) {
			m_bIsOpen = TRUE;
		} else {
			m_bIsOpen = FALSE;
		}
	} else if(key.CompareNoCase(_T("type")) == 0) {
		if(val.CompareNoCase(_T("file")) == 0) {
			m_bIsFileDlg = TRUE;
		} else {
			m_bIsFileDlg = FALSE;
		}
	}
}