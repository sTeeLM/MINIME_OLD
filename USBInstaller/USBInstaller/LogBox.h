#pragma once


// CLogBox

class CLogBox : public CRichEditCtrl
{
	DECLARE_DYNAMIC(CLogBox)

	enum LogType {
		LOG_INFO = 0,
		LOG_ERR = 1,
		LOG_WARN = 2,
		LOG_DBG  = 3
	};

public:
	CLogBox();
	virtual ~CLogBox();
	void Log(LogType eType, LPCTSTR szFmt, ...);
	void Initialize();
protected:
	void InsertText(LPCTSTR text, COLORREF color, bool bold, bool italic);
	CMutex m_Lock;
	DECLARE_MESSAGE_MAP()
};


