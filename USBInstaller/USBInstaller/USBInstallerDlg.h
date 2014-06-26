
// USBInstallerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#include "SwitchButton.h"
#include "LogBox.h"
#include "afxcmn.h"
#include "FileProgress.h"

// CUSBInstallerDlg 对话框
class CUSBInstallerDlg : public CDialog
{
// 构造
public:
	CUSBInstallerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_USBINSTALLER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	BOOL m_bWaitForClose;

	CString m_strRoot;
	CString m_strSyslinuxDir;
	LONGLONG m_nRootSize;

	BOOL m_bCancelBurn;
	HANDLE m_hBurnUSBThread;

	CComboBox m_ctlDevice;
	CStatic m_ctlRootPath;
	CButton m_btnSelRoot;
	CLogBox m_ctlLog;
	CSwitchButton m_btnStartStop;
	CFileProgress m_ctlProgress;

	void ResetBurn();

	static CString HumanSize(LONGLONG Size);
	static void GetSyslinuxPath(LPCTSTR szPath, CUSBInstallerDlg * _this);
	static LONGLONG EnumRootDir(CString & strPath, CUSBInstallerDlg * _this);
	static usb_device_info_t * OpenCurentUSB(CUSBInstallerDlg * _this, CString & strName);
	static void CloseUSB(usb_device_info_t * device, CUSBInstallerDlg * _this);
	static BOOL USBCopyFile(LPCTSTR szSrc, LPCTSTR szDst, CUSBInstallerDlg * _this);
	static DWORD WINAPI fnBurnUSB(LPVOID lpParameter);
	static BOOL CopySingleFile(LPCTSTR szSrc, LPCTSTR szDst, CUSBInstallerDlg * _this);
	static BOOL DetectPartSize(usb_device_info_t *device, DWORD * szt, CUSBInstallerDlg * _this);
	void StartBurnUSB();
	LRESULT OnBurnUSBStop(WPARAM wp, LPARAM lp);
	LRESULT OnUpdateBurnUSB(WPARAM wp, LPARAM lp);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	virtual void OnCancel();
	afx_msg void OnClose(); 
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeComboDevice();
	afx_msg void OnBnClickedButtonSelRoot();
	afx_msg void OnBnClickedButtonStartStop();
};
