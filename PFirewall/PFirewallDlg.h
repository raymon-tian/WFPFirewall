
// PFirewallDlg.h : 头文件
//

#pragma once

#include "common.h"

#define SUBLAYER_NAME L"SUBLAYER_NAME"
#define SUBLAYER_DISPLAY_NAME L"SUBLAYER_DISPLAY_NAME"


// CPFirewallDlg 对话框
class CPFirewallDlg : public CDialogEx
{
// 构造
public:
	CPFirewallDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PFIREWALL_DIALOG };
#endif

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
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
public:
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButtonStartFilter();
	afx_msg void OnBnClickedButton8();
};
