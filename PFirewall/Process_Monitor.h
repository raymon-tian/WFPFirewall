#pragma once


// Process_Monitor 对话框

class Process_Monitor : public CDialogEx
{
	DECLARE_DYNAMIC(Process_Monitor)

public:
	Process_Monitor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Process_Monitor();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = Process_Monitor };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
