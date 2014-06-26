#pragma once


// CFileProgress

class CFileProgress : public CProgressCtrl
{
	DECLARE_DYNAMIC(CFileProgress)

public:
	CFileProgress();
	virtual ~CFileProgress();
	void SetTotal(ULONGLONG nTotal);
	void Reset();
	void UpdateProgress(DWORD nSize);
protected:
	ULONGLONG m_nTotal;
	ULONGLONG m_nProgress;
	DECLARE_MESSAGE_MAP()
};


