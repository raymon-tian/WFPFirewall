#pragma once


// ProcessMonitor 对话框

class ProcessMonitor : public CDialogEx
{
	DECLARE_DYNAMIC(ProcessMonitor)

public:
	ProcessMonitor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ProcessMonitor();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ProcessMonitor };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedBtngetfolder();
	void OnBnClickedBtnstart();
	void OnBnClickedBtnstop();
	void OnBnClickedBtnaddcallouts();
	void OnBnClickedBtndelcallouts();
};
