// IPFilter.cpp : 实现文件
//

#include "stdafx.h"
#include "PFirewall.h"
#include "IPFilter.h"
#include "afxdialogex.h"
#include "PFirewallDlg.h"



// 过滤引擎句柄声明
extern HANDLE comEngineHanle;
// subLayer GUID
extern GUID subLayerGUID;
// 过滤信息vector
std::vector<std::pair<UINT64, FILTER_INFO>> filter_info_v;

// IPFilter 对话框
IMPLEMENT_DYNAMIC(IPFilter, CDialogEx)

IPFilter::IPFilter(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_IPFilter, pParent)
{

}

IPFilter::~IPFilter()
{

}

void IPFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL IPFilter::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CComboBox* pCombobox1;
	pCombobox1 = (CComboBox*)GetDlgItem(IDC_COMBO1);
	pCombobox1->AddString(_T("TCP"));
	pCombobox1->AddString(_T("UDP"));
	pCombobox1->AddString(_T("ICMP"));
	pCombobox1->AddString(_T("IP"));
	pCombobox1->SetCurSel(0);

	CComboBox* pCombobox2;
	pCombobox2 = (CComboBox*) GetDlgItem(IDC_COMBO2);
	pCombobox2->AddString(_T("permit"));
	pCombobox2->AddString(_T("block"));
	pCombobox2->SetCurSel(0);

	CComboBox* pCombobox3;
	pCombobox3 = (CComboBox*)GetDlgItem(IDC_COMBO3);
	pCombobox3->AddString(_T("in"));
	pCombobox3->AddString(_T("out"));
	pCombobox3->SetCurSel(0);

	return 0;
}


BEGIN_MESSAGE_MAP(IPFilter, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &IPFilter::OnBnClickedButton1)
END_MESSAGE_MAP()


/*
*数据格式转换
*/

UINT32 TransformFormat(CString strIP) {
	UINT32 uint32IPN, uint32IPH;
	if (!InetPton(AF_INET, strIP, &uint32IPN)) {
		printf(" ip address convertion failed\n");
	}
	uint32IPH = ntohl(uint32IPN);

	return uint32IPH;
}

void IPFilter::getFilterInfo(FILTER_INFO &info) {
	CEdit* pEditSourIP;
	pEditSourIP = (CEdit*)GetDlgItem(IDC_EDIT1);
	CString SourIP;
	pEditSourIP->GetWindowText(SourIP);
	info.str_source_ip = SourIP;

	CEdit* pEditDestIP;
	pEditDestIP = (CEdit*)GetDlgItem(IDC_EDIT3);
	CString DestIP;
	pEditDestIP->GetWindowText(DestIP);
	info.str_dest_ip = DestIP;

	CEdit* pEditSourPort;
	pEditSourPort = (CEdit*)GetDlgItem(IDC_EDIT5);
	CString SourPort;
	pEditSourPort->GetWindowText(SourPort);
	info.str_source_port = SourPort;

	CEdit* pEditDestPort;
	pEditDestPort = (CEdit*)GetDlgItem(IDC_EDIT6);
	CString DestPort;
	pEditDestPort->GetWindowText(DestPort);
	info.str_dest_port = DestPort;

	CComboBox* pComboType;
	pComboType = (CComboBox*)GetDlgItem(IDC_COMBO1);
	CString Type;
	pComboType->GetWindowText(Type);
	info.str_protocol_type = Type;

	CComboBox* pComboaction;
	pComboaction = (CComboBox*)GetDlgItem(IDC_COMBO2);
	CString action;
	pComboaction->GetWindowText(action);
	info.str_action = action;

	CComboBox* pCombodirection;
	pCombodirection = (CComboBox*)GetDlgItem(IDC_COMBO3);
	CString direction;
	pCombodirection->GetWindowText(direction);
	info.str_direction = direction;

	/*
	CString str;
	str.Format(L"di %s si %s dp %s sp %s type %s dire %s act %s", DestIP, SourIP, DestPort, SourPort, Type, direction, action);
	MessageBox(str, _T("test"), MB_OK);*/

	if(SourIP!="")	info.source_ip = TransformFormat(SourIP);
	else	info.source_ip = 0;
	if(DestIP!="")	info.dest_ip = TransformFormat(DestIP);
	else	info.dest_ip = 0;
	if(SourPort!="")	info.source_port = _tstoi(SourPort);
	else info.source_port = 0;
	if(DestPort!="")	info.dest_port = _tstoi(DestPort);
	else	info.dest_port = 0;
	if (action == L"permit") {
		info.action = PERMIT;
	}
	else {
		info.action = BLOCK;
	}
	if (Type == "TCP") {
		info.protocol_type = TCP_PROTOCOL;
	}
	if (Type == "UDP") {
		info.protocol_type = UDP_PROTOCOL;
	}
	if (Type == "ICMP") {
		info.protocol_type = ICMP_PROTOCOL;
	}
	if (Type == "IP") {
		info.protocol_type = IP_PROTOCOL;
	}
	if (direction == "in") {
		info.direction = IN;
	}
	else {
		info.direction = OUT;
	}
	/*
	CString str;
	str.Format(L"sp %d dp %d", info.source_port,info.dest_port);
	MessageBox(str, _T("test"), MB_OK);*/
}

// 添加一条过滤规则
void IPFilter::OnBnClickedButton1()
{
	FILTER_INFO info = {0};
	getFilterInfo(info);
	filter_info_v.push_back({ 0, info });
	MessageBox(L"过滤规则添加成功", _T("SUCCESS"), MB_OK);
	EndDialog(0);
}
