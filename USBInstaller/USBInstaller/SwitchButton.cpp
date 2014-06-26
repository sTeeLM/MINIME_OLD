// SwitchButton.cpp : 实现文件
//

#include "stdafx.h"
#include "USBInstaller.h"
#include "SwitchButton.h"


// CSwitchButton

IMPLEMENT_DYNAMIC(CSwitchButton, CButton)

CSwitchButton::CSwitchButton()
{
	m_nIndex = 0;
	m_strCap1 = _T("");
	m_strCap2 = _T("");
}

CSwitchButton::~CSwitchButton()
{
}

void CSwitchButton::SetCaption(LPCTSTR szCap1, LPCTSTR szCap2)
{
	m_strCap1 = szCap1;
	m_strCap2 = szCap2;
	m_nIndex = 0;
	SetWindowText(m_strCap1);
}

void CSwitchButton::Switch(INT nIndex /* = -1*/)
{
	if(nIndex < 0) { 
		if(m_nIndex == 0) {
			m_nIndex = 1;
			SetWindowText(m_strCap2);
		} else {
			m_nIndex = 0;
			SetWindowText(m_strCap1);
		}
	} else {
		m_nIndex = nIndex;
		SetWindowText( m_nIndex == 0 ? m_strCap1 : m_strCap2);
	}
}

BEGIN_MESSAGE_MAP(CSwitchButton, CButton)
END_MESSAGE_MAP()



// CSwitchButton 消息处理程序


