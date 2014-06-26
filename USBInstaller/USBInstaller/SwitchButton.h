#pragma once


// CSwitchButton

class CSwitchButton : public CButton
{
	DECLARE_DYNAMIC(CSwitchButton)

public:
	CSwitchButton();
	virtual ~CSwitchButton();
	void SetCaption(LPCTSTR szCap1, LPCTSTR szCap2);
	void Switch(INT nIndex = -1);
	int GetSwitch(){
		return m_nIndex;
	}
protected:
	int m_nIndex;
	CString m_strCap1;
	CString m_strCap2;
	DECLARE_MESSAGE_MAP()
};


