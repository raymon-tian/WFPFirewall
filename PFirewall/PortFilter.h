#pragma once


// PortFilter 对话框

class PortFilter : public CDialogEx
{
	DECLARE_DYNAMIC(PortFilter)

public:
	PortFilter(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~PortFilter();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PortFilter };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
