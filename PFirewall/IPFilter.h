#pragma once

#include <vector>
#include <utility>
#include "common.h"

// 协议类型
#define IP_PROTOCOL 0
#define ICMP_PROTOCOL 1
#define TCP_PROTOCOL 2
#define UDP_PROTOCOL 3
// 方向
#define IN 1
#define OUT 0
// 动作
#define PERMIT 1
#define BLOCK 0
#define FILTER_DISPLAY_NAME L"filter display name PFirewall"

// IPFilter 对话框

// 过滤信息
typedef struct _FILTER_INFO {
	UINT32 source_ip;//源IP
	UINT32 dest_ip;//目的IP
	UINT16 source_port;//源端口
	UINT16 dest_port;//目的IP
	UINT16 protocol_type;//协议类型
	UINT16 direction;//数据包的方向
	UINT16 action;//过滤动作
	CString str_source_ip;
	CString str_dest_ip;
	CString str_source_port;
	CString str_dest_port;
	CString str_protocol_type;
	CString str_direction;
	CString str_action;
}FILTER_INFO;

class IPFilter : public CDialogEx
{
	DECLARE_DYNAMIC(IPFilter)

public:
	IPFilter(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~IPFilter();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	void getFilterInfo(FILTER_INFO &);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
