// SHLSELDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CSHLSELDlg �Ի���
class CSHLSELDlg : public CDialog
{
// ����
public:
	CSHLSELDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SHLSEL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void RunProcess(SHELConfig * pCfg);
	static DWORD WINAPI fnMon(LPVOID lpParameter);
	static HWND m_hWndMaster;
public:
	afx_msg void OnBnClickedCancel();
public:
	BOOL m_bShel2;
public:
	afx_msg void OnBnClickedOk();
};
