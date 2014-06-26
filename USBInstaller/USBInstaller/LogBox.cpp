// LogBox.cpp : 实现文件
//

#include "stdafx.h"
#include "USBInstaller.h"
#include "LogBox.h"
#include <stdarg.h>

#define MAX_LOG_LINE 4096

// CLogBox

IMPLEMENT_DYNAMIC(CLogBox, CRichEditCtrl)

CLogBox::CLogBox()
{

}

CLogBox::~CLogBox()
{
}


BEGIN_MESSAGE_MAP(CLogBox, CRichEditCtrl)
END_MESSAGE_MAP()

void CLogBox::Initialize()
{
	ModifyStyle(0,ES_MULTILINE,0);
}

// CLogBox 消息处理程序
void CLogBox::Log(LogType eType, LPCTSTR szFmt, ...)
{
	TCHAR Buffer[1024];
	COLORREF color;
	int len = 0;
	va_list argptr;
	va_start(argptr, szFmt);
	len = _vstprintf_s(Buffer, _countof(Buffer) - 3, szFmt, argptr);
	va_end( argptr);
	
	Buffer[len] = _T('\n');
	Buffer[len+1] = 0;

	if(eType == LOG_INFO) {
		color = RGB(128,255,128);
	} else if(eType == LOG_ERR) {
		color = RGB(255,0,0);
	} else if(eType == LOG_WARN){
		color = RGB(255,255,0);
	} else {
		color = RGB(0,0,0);
	}

	InsertText(Buffer, color ,0 ,0);
}

void CLogBox::InsertText(LPCTSTR text, COLORREF color, bool bold, bool italic)
{
    CHARFORMAT cf = {0};
    int nLen = GetTextLength();
	int nTemp;
	CString _text = text;

	m_Lock.Lock();

	if(MAX_LOG_LINE < GetLineCount()) {
		nTemp = GetLineCount();
		nTemp = LineIndex(1);
		SetSel(0, nLen);
		ReplaceSel(_T(""));
		nLen = GetTextLength();
	}

    cf.cbSize = sizeof(cf);
    cf.dwMask = (bold ? CFM_BOLD : 0) | (italic ? CFM_ITALIC : 0) | CFM_COLOR;
    cf.dwEffects = (bold ? CFE_BOLD : 0) | (italic ? CFE_ITALIC : 0) |~CFE_AUTOCOLOR;
    cf.crTextColor = color;

    SetSel(nLen, -1); // Set the cursor to the end of the text area and deselect everything.
    ReplaceSel(text); // Inserts when nothing is selected.
	//ReplaceSel((txtLen < 1 ? L"" : L"\n") + _text);
    // Apply formating to the just inserted text.
	if(nLen == 0)
		SetSel(nLen, -1 );
	else
		SetSel(nLen - GetLineCount() + 1, -1 );
    SetSelectionCharFormat(cf);
	LineScroll(GetLineCount(), 0);
	m_Lock.Unlock();
}
