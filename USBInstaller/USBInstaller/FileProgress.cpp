// FileProgress.cpp : 实现文件
//

#include "stdafx.h"
#include "USBInstaller.h"
#include "FileProgress.h"


// CFileProgress

IMPLEMENT_DYNAMIC(CFileProgress, CProgressCtrl)

CFileProgress::CFileProgress()
{
	m_nTotal = m_nProgress = 0;
}

CFileProgress::~CFileProgress()
{

}


BEGIN_MESSAGE_MAP(CFileProgress, CProgressCtrl)
END_MESSAGE_MAP()



// CFileProgress 消息处理程序
void CFileProgress::SetTotal(ULONGLONG nTotal)
{
	m_nTotal = nTotal;
}

void CFileProgress::Reset()
{
	m_nTotal = m_nProgress = 0;
	SetRange(0, 100);
	SetPos(0);
}
void CFileProgress::UpdateProgress(DWORD nSize)
{
	int nProgress;
	m_nProgress +=nSize;
	if(m_nTotal != 0) {
		nProgress = (int)((m_nProgress * 100)/m_nTotal);
		SetPos(nProgress);
	}
}

