// Alert.cpp : 实现文件
//

#include "stdafx.h"
#include "PFirewall.h"
#include "Alert.h"
#include "afxdialogex.h"

#include <windows.h>
#include <fwpmu.h>
#include <stdio.h>
#include <rpc.h>

#define SESSION_NAME L"My Session Name"
#define PROVIDER_NAME L"My Provider Name"
#define SUBLAYER_NAME L"My Sublayer Name"
#define FILTER_NAME L"My Filter Name"

#pragma comment(lib, "fwpuclnt.lib")
#pragma comment(lib, "Rpcrt4.lib")

#define EXIT_ON_ERROR(fnName) \
   if (result != ERROR_SUCCESS) \
   { \
     strError.Format(L"%s failed (%08x)", fnName,result);\
	 MessageBox(strError,_T("ERROR"),MB_OK);\
     FwpmEngineClose0(engine);\
     EndDialog(0);\
     return;\
   }


CString strError;
DWORD result = ERROR_SUCCESS;
HANDLE engine = NULL;
FWPM_SESSION0 session;
FWPM_PROVIDER0 provider;
FWPM_SUBLAYER0 subLayer;

FWPM_FILTER_CONDITION0 conds[1];
FWPM_FILTER0 filter;
BOOL txnInProgress = FALSE;

// 5fb216a8-e2e8-4024-b853-391a4168641e
const GUID PROVIDER_KEY =
{
	0x5fb216a8,
	0xe2e8,
	0x4024,
	{ 0xb8, 0x53, 0x39, 0x1a, 0x41, 0x68, 0x64, 0x1e }
};

// 5fb216a9-e2e8-4024-b853-391a4168641e
const GUID SUBLAYER_KEY =
{
	0x5fb216a9,
	0xe2e8,
	0x4024,
	{ 0xb8, 0x53, 0x39, 0x1a, 0x41, 0x68, 0x64, 0x1e }
};


void Alert::InstallProvider(
	__in const GUID* providerKey,
	__in PCWSTR providerName,
	__in const GUID* subLayerKey,
	__in PCWSTR subLayerName
)
{
	memset(&session, 0, sizeof(session));
	// The session name isn't required but may be useful for diagnostics.
	session.displayData.name = SESSION_NAME;
	// Set an infinite wait timeout, so we don't have to handle FWP_E_TIMEOUT
	// errors while waiting to acquire the transaction lock.
	session.txnWaitTimeoutInMSec = INFINITE;

	// The authentication service should always be RPC_C_AUTHN_DEFAULT.
	result = FwpmEngineOpen0(
		NULL,
		RPC_C_AUTHN_WINNT,
		NULL,
		&session,
		&engine
	);
	if (result != ERROR_SUCCESS)
	{
		CString str;
		str.Format(L"FwpmEngineOpen0 failed (%08x)", result);
		MessageBox(str, _T("ERROR"), MB_OK);
		return;
	}
	result = FwpmTransactionBegin0(engine, 0);
	EXIT_ON_ERROR(FwpmTransactionBegin0);

	memset(&provider, 0, sizeof(provider));
	provider.providerKey = *providerKey;
	provider.displayData.name = (PWSTR)providerName;
	provider.flags = FWPM_PROVIDER_FLAG_PERSISTENT;

	result = FwpmProviderAdd0(engine, &provider, NULL);
	if (result != FWP_E_ALREADY_EXISTS)
	{
		EXIT_ON_ERROR(FwpmProviderAdd0);
	}

	memset(&subLayer, 0, sizeof(subLayer));
	subLayer.subLayerKey = *subLayerKey;
	subLayer.displayData.name = (PWSTR)subLayerName;
	subLayer.flags = FWPM_SUBLAYER_FLAG_PERSISTENT;
	
	subLayer.providerKey = (GUID*)providerKey;
	
	subLayer.weight = 0x8000;

	result = FwpmSubLayerAdd0(engine, &subLayer, NULL);
	if (result != FWP_E_ALREADY_EXISTS)
	{
		EXIT_ON_ERROR(FwpmSubLayerAdd0);
	}
	result = FwpmTransactionCommit0(engine);
	if (result != ERROR_SUCCESS)
	{
		CString str;
		str.Format(L"FwpmTransactionCommit0 failed (%08x)", result);
		MessageBox(str, _T("ERROR"), MB_OK);
		return;
	}
}

void Alert::BlockPortScanning(
	__in HANDLE engine,
	__in PCWSTR filterName,
	__in_opt const GUID* providerKey,
	__in_opt const GUID* subLayerKey
)
{
	memset(&filter, 0, sizeof(filter));
	filter.displayData.name = (PWSTR)filterName;
	filter.providerKey = (GUID*)providerKey;
	if (subLayerKey != NULL)
	{
		filter.subLayerKey = *subLayerKey;
	}
	filter.numFilterConditions = 1;
	filter.filterCondition = conds;

	result = FwpmTransactionBegin0(engine, 0);
	EXIT_ON_ERROR(FwpmTransactionBegin0);
	txnInProgress = TRUE;

	conds[0].fieldKey = FWPM_CONDITION_ICMP_TYPE;
	conds[0].matchType = FWP_MATCH_EQUAL;
	conds[0].conditionValue.type = FWP_UINT16;
	//根据ICMP报文类型的值来确定，譬如：3为终点不可达；11位时间超时；8为回送请求；0为回送应答
	conds[0].conditionValue.uint16 = 8;

	filter.action.type = FWP_ACTION_BLOCK;

	filter.layerKey = FWPM_LAYER_OUTBOUND_ICMP_ERROR_V4;
	result = FwpmFilterAdd0(engine, &filter, NULL, NULL);
	EXIT_ON_ERROR(FwpmFilterAdd0);

	filter.layerKey = FWPM_LAYER_OUTBOUND_ICMP_ERROR_V6;
	result = FwpmFilterAdd0(engine, &filter, NULL, NULL);
	EXIT_ON_ERROR(FwpmFilterAdd0);

	conds[0].fieldKey = FWPM_CONDITION_FLAGS;
	conds[0].matchType = FWP_MATCH_FLAGS_NONE_SET;
	conds[0].conditionValue.type = FWP_UINT32;
	conds[0].conditionValue.uint32 = FWP_CONDITION_FLAG_IS_LOOPBACK;

	filter.action.type = FWP_ACTION_CALLOUT_TERMINATING;

	filter.layerKey = FWPM_LAYER_INBOUND_TRANSPORT_V4_DISCARD;
	filter.action.calloutKey = FWPM_CALLOUT_WFP_TRANSPORT_LAYER_V4_SILENT_DROP;
	result = FwpmFilterAdd0(engine, &filter, NULL, NULL);
	EXIT_ON_ERROR(FwpmFilterAdd0);

	filter.layerKey = FWPM_LAYER_INBOUND_TRANSPORT_V6_DISCARD;
	filter.action.calloutKey = FWPM_CALLOUT_WFP_TRANSPORT_LAYER_V6_SILENT_DROP;
	result = FwpmFilterAdd0(engine, &filter, NULL, NULL);
	EXIT_ON_ERROR(FwpmFilterAdd0);

	result = FwpmTransactionCommit0(engine);
	EXIT_ON_ERROR(FwpmTransactionCommit0);
	txnInProgress = FALSE;

	if (txnInProgress)
	{
		printf("error in xxxxx");
		FwpmTransactionAbort0(engine);
	}
	printf("Block success");
}

// Alert 对话框

IMPLEMENT_DYNAMIC(Alert, CDialogEx)

extern UINT32 TransformFormat(CString strIP);
Alert::Alert(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_Alert, pParent)
{

}

Alert::~Alert()
{
}

void Alert::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Alert, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &Alert::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &Alert::OnBnClickedButton3)
END_MESSAGE_MAP()


// Alert 消息处理程序


// 启动网络报警
void Alert::OnBnClickedButton1()
{
	InstallProvider(&PROVIDER_KEY, PROVIDER_NAME, &SUBLAYER_KEY, SUBLAYER_NAME);
	BlockPortScanning(engine, FILTER_NAME, &provider.providerKey, &subLayer.subLayerKey);
	MessageBox(L"开启成功", _T("SUCCESS"), MB_OK);
	CButton *pBtnStart, *pBtnStop;
	pBtnStart = (CButton*)GetDlgItem(IDC_BUTTON1);
	pBtnStop = (CButton*)GetDlgItem(IDC_BUTTON3);
	pBtnStart->EnableWindow(FALSE);
	pBtnStop->EnableWindow(TRUE);
}

// 关闭网络报警
void Alert::OnBnClickedButton3()
{
	FwpmEngineClose0(engine);
	MessageBox(L"关闭成功", _T("SUCCESS"), MB_OK);
	CButton *pBtnStart, *pBtnStop;
	pBtnStart = (CButton*)GetDlgItem(IDC_BUTTON1);
	pBtnStop = (CButton*)GetDlgItem(IDC_BUTTON3);
	pBtnStart->EnableWindow(TRUE);
	pBtnStop->EnableWindow(FALSE);
}

