// SHLSELDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CSHLSELDlg 对话框
class CSHLSELDlg : public CDialog
{
// 构造
public:
	CSHLSELDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SHLSEL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
