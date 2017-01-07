// ProcessMonitor.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PFirewall.h"
#include "ProcessMonitor.h"
#include "afxdialogex.h"
#include "windows.h"
#include "winioctl.h"
#include "strsafe.h"

#ifndef _CTYPE_DISABLE_MACROS
#define _CTYPE_DISABLE_MACROS
#endif

#include "fwpmu.h"
#include "winsock2.h"
#include "ws2def.h"
#include <conio.h>
#include <stdio.h>
//#include "ioctl.h"
#define INITGUID
#include <guiddef.h>
//#include "mntrguid.h"

//ioctl.h������
#define MONITOR_FLOW_ESTABLISHED_CALLOUT_DESCRIPTION L"Monitor Sample - Flow Established Callout"
#define MONITOR_FLOW_ESTABLISHED_CALLOUT_NAME L"Flow Established Callout"

#define MONITOR_STREAM_CALLOUT_DESCRIPTION L"Monitor Sample - Stream Callout"
#define MONITOR_STREAM_CALLOUT_NAME L"Stream Callout"

#define MONITOR_DEVICE_NAME     L"\\Device\\MonitorSample"
#define MONITOR_SYMBOLIC_NAME   L"\\DosDevices\\Global\\MonitorSample"
#define MONITOR_DOS_NAME   L"\\\\.\\MonitorSample"

typedef enum _MONITOR_OPERATION_MODE
{
	invalidOperation = 0,
	monitorTraffic = 1,
	monitorOperationMax
} MONITOR_OPERATION_MODE;

typedef struct _MONITOR_SETTINGS
{
	MONITOR_OPERATION_MODE  monitorOperation;
	UINT32                  flags;
} MONITOR_SETTINGS;

#define	MONITOR_IOCTL_ENABLE_MONITOR  CTL_CODE(FILE_DEVICE_NETWORK, 0x1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	MONITOR_IOCTL_DISABLE_MONITOR CTL_CODE(FILE_DEVICE_NETWORK, 0x2, METHOD_BUFFERED, FILE_ANY_ACCESS)

//mntrguid.h������
// b3241f1d-7cd2-4e7a-8721-2e97d07702e5
DEFINE_GUID(
	MONITOR_SAMPLE_SUBLAYER,
	0xb3241f1d,
	0x7cd2,
	0x4e7a,
	0x87, 0x21, 0x2e, 0x97, 0xd0, 0x77, 0x02, 0xe5
);

// 3aaccbc0-2c29-455f-bb91-0e801c8994a4
DEFINE_GUID(
	MONITOR_SAMPLE_FLOW_ESTABLISHED_CALLOUT_V4,
	0x3aaccbc0,
	0x2c29,
	0x455f,
	0xbb, 0x91, 0x0e, 0x80, 0x1c, 0x89, 0x94, 0xa4
);

// cea0131a-6ed3-4ed6-b40c-8a8fe8434b0a
DEFINE_GUID(
	MONITOR_SAMPLE_STREAM_CALLOUT_V4,
	0xcea0131a,
	0x6ed3,
	0x4ed6,
	0xb4, 0x0c, 0x8a, 0x8f, 0xe8, 0x43, 0x4b, 0x0a
);

HANDLE quitEvent;

// ProcessMonitor �Ի���

IMPLEMENT_DYNAMIC(ProcessMonitor, CDialogEx)

ProcessMonitor::ProcessMonitor(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ProcessMonitor, pParent)
{

}

ProcessMonitor::~ProcessMonitor()
{
}

void ProcessMonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ProcessMonitor, CDialogEx)	
	ON_BN_CLICKED(IDC_Btngetfolder, &ProcessMonitor::OnBnClickedBtngetfolder)
	ON_BN_CLICKED(IDC_BtnStart, &ProcessMonitor::OnBnClickedBtnstart)
	ON_BN_CLICKED(IDC_BtnStop, &ProcessMonitor::OnBnClickedBtnstop)
	ON_BN_CLICKED(IDC_BtnAddcallouts, &ProcessMonitor::OnBnClickedBtnaddcallouts)
	ON_BN_CLICKED(IDC_BtnDelcallouts, &ProcessMonitor::OnBnClickedBtndelcallouts)
END_MESSAGE_MAP()


// ProcessMonitor ��Ϣ�������

DWORD MonitorAppOpenMonitorDevice(_Out_ HANDLE* monitorDevice)
	/*++

	Routine Description:

	Opens the Monitor Sample monitorDevice

	Arguments:

	[out] HANDLE* monitorDevice

	Return Value:

	NO_ERROR, ERROR_INVALID_PARAMETER or a CreateFile specific result.

	--*/
{
	if (!monitorDevice)
	{
		return ERROR_INVALID_PARAMETER;
	}
	*monitorDevice = CreateFileW(MONITOR_DOS_NAME,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (*monitorDevice == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	return NO_ERROR;
}

BOOL MonitorAppCloseMonitorDevice(_In_ HANDLE monitorDevice)
	/*++

	Routine Description:

	Closes the Monitor Sample monitorDevice

	Arguments:

	Return Value:

	None.

	--*/
{
	return CloseHandle(monitorDevice);
}

DWORD MonitorAppAddCallouts()
/*++

Routine Description:

Adds the callouts during installation

Arguments:

[in]  PCWSTR AppPath - The path to the application to monitor.

Return Value:

NO_ERROR or a specific FWP result.

--*/
{
	FWPM_CALLOUT callout;
	DWORD result;
	FWPM_DISPLAY_DATA displayData;
	HANDLE engineHandle = NULL;
	FWPM_SESSION session;
	RtlZeroMemory(&session, sizeof(FWPM_SESSION));

	session.displayData.name = L"Monitor Sample Non-Dynamic Session";
	session.displayData.description = L"For Adding callouts";

	//MessageBox("Opening Filtering Engine");
	::MessageBox(NULL, _T("Opening Filtering Engine"), _T("demo"), MB_OK);
	result = FwpmEngineOpen(
		NULL,
		RPC_C_AUTHN_WINNT,
		NULL,
		&session,
		&engineHandle
	);

	if (NO_ERROR != result)
	{
		goto cleanup;
	}

	//printf("Starting Transaction for adding callouts\n");
	::MessageBox(NULL, _T("Starting Transaction for adding callouts"), _T("demo"), MB_OK);
	result = FwpmTransactionBegin(engineHandle, 0);
	if (NO_ERROR != result)
	{
		goto abort;
	}

	::MessageBox(NULL,_T("Successfully started the Transaction"),_T("demo"),MB_OK);

	RtlZeroMemory(&callout, sizeof(FWPM_CALLOUT));
	displayData.description = MONITOR_FLOW_ESTABLISHED_CALLOUT_DESCRIPTION;
	displayData.name = MONITOR_FLOW_ESTABLISHED_CALLOUT_NAME;

	callout.calloutKey = MONITOR_SAMPLE_FLOW_ESTABLISHED_CALLOUT_V4;
	callout.displayData = displayData;
	callout.applicableLayer = FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4;
	callout.flags = FWPM_CALLOUT_FLAG_PERSISTENT; // Make this a persistent callout.

	//printf("Adding Persistent Flow Established callout through the Filtering Engine\n");
	::MessageBox(NULL, _T("Adding Persistent Flow Established callout through the Filtering Engine"), _T("demo"), MB_OK);

	result = FwpmCalloutAdd(engineHandle, &callout, NULL, NULL);
	if (NO_ERROR != result)
	{
		goto abort;
	}

	//printf("Successfully Added Persistent Flow Established callout.\n");
	::MessageBox(NULL, _T("Successfully Added Persistent Flow Established callout."), _T("demo"), MB_OK);

	RtlZeroMemory(&callout, sizeof(FWPM_CALLOUT));

	displayData.description = MONITOR_STREAM_CALLOUT_DESCRIPTION;
	displayData.name = MONITOR_STREAM_CALLOUT_DESCRIPTION;

	callout.calloutKey = MONITOR_SAMPLE_STREAM_CALLOUT_V4;
	callout.displayData = displayData;
	callout.applicableLayer = FWPM_LAYER_STREAM_V4;
	callout.flags = FWPM_CALLOUT_FLAG_PERSISTENT; // Make this a persistent callout.

	//printf("Adding Persistent Stream callout through the Filtering Engine\n");
	::MessageBox(NULL, _T("Adding Persistent Stream callout through the Filtering Engine"), _T("demo"), MB_OK);

	result = FwpmCalloutAdd(engineHandle, &callout, NULL, NULL);
	if (NO_ERROR != result)
	{
		goto abort;
	}

	//printf("Successfully Added Persistent Stream callout.\n");
	::MessageBox(NULL, _T("Successfully Added Persistent Stream callout."), _T("demo"), MB_OK);

	printf("Committing Transaction\n");
	::MessageBox(NULL, _T("Committing Transaction��"), _T("demo"), MB_OK);

	result = FwpmTransactionCommit(engineHandle);
	if (NO_ERROR == result)
	{
		//printf("Successfully Committed Transaction.\n");
		::MessageBox(NULL, _T("Successfully Committed Transaction."), _T("demo"), MB_OK);
	}
	goto cleanup;

abort:
	//printf("Aborting Transaction\n");
	::MessageBox(NULL, _T("Aborting Transaction"), _T("demo"), MB_OK);
	result = FwpmTransactionAbort(engineHandle);
	if (NO_ERROR == result)
	{
		//printf("Successfully Aborted Transaction.\n");
		::MessageBox(NULL, _T("Successfully Aborted Transaction."), _T("demo"), MB_OK);
	}

cleanup:

	if (engineHandle)
	{
		FwpmEngineClose(engineHandle);
	}
	return result;
}

DWORD MonitorAppRemoveCallouts()
/*++

Routine Description:

Sets the kernel callout ID's through the Monitor Sample device

Arguments:

[in] HANDLE monitorDevice - Monitor Sample device
[in] CALLOUTS* callouts - Callout structure with ID's set
[in] DWORD size - Size of the callout structure.

Return Value:

NO_ERROR or a specific DeviceIoControl result.

--*/
{
	DWORD result;
	HANDLE engineHandle = NULL;
	FWPM_SESSION session;

	RtlZeroMemory(&session, sizeof(FWPM_SESSION));

	session.displayData.name = L"Monitor Sample Non-Dynamic Session";
	session.displayData.description = L"For Adding callouts";

	//printf("Opening Filtering Engine\n");
	result = FwpmEngineOpen(
		NULL,
		RPC_C_AUTHN_WINNT,
		NULL,
		&session,
		&engineHandle
	);

	if (NO_ERROR != result)
	{
		goto cleanup;
	}

	//printf("Starting Transaction for Removing callouts\n");

	result = FwpmTransactionBegin(engineHandle, 0);
	if (NO_ERROR != result)
	{
		goto abort;
	}
	//printf("Successfully started the Transaction\n");

	//printf("Deleting Flow Established callout\n");
	result = FwpmCalloutDeleteByKey(engineHandle,
		&MONITOR_SAMPLE_FLOW_ESTABLISHED_CALLOUT_V4);
	if (NO_ERROR != result)
	{
		goto abort;
	}

	//printf("Successfully Deleted Flow Established callout\n");

	//printf("Deleting Stream callout\n");

	result = FwpmCalloutDeleteByKey(engineHandle,
		&MONITOR_SAMPLE_STREAM_CALLOUT_V4);
	if (NO_ERROR != result)
	{
		goto abort;
	}
	//printf("Successfully Deleted Stream callout\n");

	//printf("Committing Transaction\n");
	result = FwpmTransactionCommit(engineHandle);
	if (NO_ERROR == result)
	{
		//printf("Successfully Committed Transaction.\n");
	}
	goto cleanup;

abort:
	//printf("Aborting Transaction\n");
	result = FwpmTransactionAbort(engineHandle);
	if (NO_ERROR == result)
	{
		//printf("Successfully Aborted Transaction.\n");
	}

cleanup:

	if (engineHandle)
	{
		FwpmEngineClose(engineHandle);
	}

	return result;
}

DWORD MonitorAppEnableMonitoring(
	_In_    HANDLE            monitorDevice,
	_In_    MONITOR_SETTINGS* monitorSettings)
	/*++

	Routine Description:

	Enables monitoring on new connections.

	Arguments:

	[in] HANDLE monitorDevice - Monitor Sample device
	[in] MONITOR_SETTINGS* monitorSettings - Settings for the Monitor Sample driver.

	Return Value:

	NO_ERROR or a specific DeviceIoControl result.

	--*/
{
	DWORD bytesReturned;

	if (!DeviceIoControl(monitorDevice,
		MONITOR_IOCTL_ENABLE_MONITOR,
		monitorSettings,
		sizeof(MONITOR_SETTINGS),
		NULL,
		0,
		&bytesReturned,
		NULL))
	{
		return GetLastError();
	}

	return NO_ERROR;
}

DWORD MonitorAppDisableMonitoring(_In_ HANDLE monitorDevice)
	/*++

	Routine Description:

	Disables monitoring of new flows (existing flows will continue to be
	monitored until the driver is stopped or the flows end).

	Arguments:

	[in] HANDLE monitorDevice - Monitor Sample device handle.

	Return Value:

	NO_ERROR or DeviceIoControl specific code.

	--*/
{
	DWORD bytesReturned;

	if (!DeviceIoControl(monitorDevice,
		MONITOR_IOCTL_DISABLE_MONITOR,
		NULL,
		0,
		NULL,
		0,
		&bytesReturned,
		NULL))
	{
		return GetLastError();
	}

	return NO_ERROR;
}

DWORD MonitorAppAddFilters(
	_In_    HANDLE         engineHandle,
	_In_    FWP_BYTE_BLOB* applicationPath)
	/*++

	Routine Description:

	Adds the required sublayer, filters and callouts to the Windows
	Filtering Platform (WFP).

	Arguments:

	[in] HANDLE engineHandle - Handle to the base Filtering engine
	[in] FWP_BYTE_BLOB* applicationPath - full path to the application including
	the NULL terminator and size also
	including the NULL the terminator
	[in] CALLOUTS* callouts - The callouts that need to be added.

	Return Value:

	NO_ERROR or a specific result

	--*/
{
	DWORD result = NO_ERROR;
	FWPM_SUBLAYER monitorSubLayer;
	FWPM_FILTER filter;
	FWPM_FILTER_CONDITION filterConditions[2]; // We only need two for this call.

	RtlZeroMemory(&monitorSubLayer, sizeof(FWPM_SUBLAYER));

	monitorSubLayer.subLayerKey = MONITOR_SAMPLE_SUBLAYER;
	monitorSubLayer.displayData.name = L"Monitor Sample Sub layer";
	monitorSubLayer.displayData.description = L"Monitor Sample Sub layer";
	monitorSubLayer.flags = 0;
	// We don't really mind what the order of invocation is.
	monitorSubLayer.weight = 0;

	//printf("Starting Transaction\n");
	::MessageBox(NULL, _T("Starting Transaction"), _T("demo"), MB_OK);

	result = FwpmTransactionBegin(engineHandle, 0);
	if (NO_ERROR != result)
	{
		goto abort;
	}
	//printf("Successfully Started Transaction\n");
	::MessageBox(NULL, _T("Successfully Started Transaction"), _T("demo"), MB_OK);
	//printf("Adding Sublayer\n");
	::MessageBox(NULL, _T("Adding Sublayer"), _T("demo"), MB_OK);

	result = FwpmSubLayerAdd(engineHandle, &monitorSubLayer, NULL);
	if (NO_ERROR != result)
	{
		goto abort;
	}

	//printf("Sucessfully added Sublayer\n");
	::MessageBox(NULL, _T("Sucessfully added Sublayer"), _T("demo"), MB_OK);

	RtlZeroMemory(&filter, sizeof(FWPM_FILTER));

	filter.layerKey = FWPM_LAYER_ALE_FLOW_ESTABLISHED_V4;
	filter.displayData.name = L"Flow established filter.";
	filter.displayData.description = L"Sets up flow for traffic that we are interested in.";
	filter.action.type = FWP_ACTION_CALLOUT_INSPECTION; // We're only doing inspection.
	filter.action.calloutKey = MONITOR_SAMPLE_FLOW_ESTABLISHED_CALLOUT_V4;
	filter.filterCondition = filterConditions;
	filter.subLayerKey = monitorSubLayer.subLayerKey;
	filter.weight.type = FWP_EMPTY; // auto-weight.

	filter.numFilterConditions = 2;

	RtlZeroMemory(filterConditions, sizeof(filterConditions));

	//
	// Add the application path to the filter conditions.
	//
	filterConditions[0].fieldKey = FWPM_CONDITION_ALE_APP_ID;
	filterConditions[0].matchType = FWP_MATCH_EQUAL;
	filterConditions[0].conditionValue.type = FWP_BYTE_BLOB_TYPE;
	filterConditions[0].conditionValue.byteBlob = applicationPath;

	//
	// For the purposes of this sample, we will monitor TCP traffic only.
	//
	filterConditions[1].fieldKey = FWPM_CONDITION_IP_PROTOCOL;
	filterConditions[1].matchType = FWP_MATCH_EQUAL;
	filterConditions[1].conditionValue.type = FWP_UINT8;
	filterConditions[1].conditionValue.uint8 = IPPROTO_TCP;

	//printf("Adding Flow Established Filter\n");
	::MessageBox(NULL, _T("Adding Flow Established Filter"), _T("demo"), MB_OK);
	result = FwpmFilterAdd(engineHandle,
		&filter,
		NULL,
		NULL);

	if (NO_ERROR != result)
	{
		goto abort;
	}

	//printf("Successfully added Flow Established filter\n");
	::MessageBox(NULL, _T("Successfully added Flow Established filter"), _T("demo"), MB_OK);
	RtlZeroMemory(&filter, sizeof(FWPM_FILTER));

	filter.layerKey = FWPM_LAYER_STREAM_V4;
	filter.action.type = FWP_ACTION_CALLOUT_INSPECTION; // We're only doing inspection.
	filter.action.calloutKey = MONITOR_SAMPLE_STREAM_CALLOUT_V4;
	filter.subLayerKey = monitorSubLayer.subLayerKey;
	filter.weight.type = FWP_EMPTY; // auto-weight.

	filter.numFilterConditions = 0;

	RtlZeroMemory(filterConditions, sizeof(filterConditions));

	filter.filterCondition = filterConditions;

	filter.displayData.name = L"Stream Layer Filter";
	filter.displayData.description = L"Monitors TCP traffic.";

	//printf("Adding Stream Filter\n");
	::MessageBox(NULL, _T("Adding Stream Filter"), _T("demo"), MB_OK);
	result = FwpmFilterAdd(engineHandle,
		&filter,
		NULL,
		NULL);

	if (NO_ERROR != result)
	{
		goto abort;
	}

	//printf("Successfully added Stream filter\n");
	::MessageBox(NULL, _T("Successfully added Stream filter"), _T("demo"), MB_OK);
	//printf("Committing Transaction\n");
	::MessageBox(NULL, _T("Committing Transaction"), _T("demo"), MB_OK);

	result = FwpmTransactionCommit(engineHandle);
	if (NO_ERROR == result)
	{
		//printf("Successfully Committed Transaction\n");
		::MessageBox(NULL, _T("Successfully Committed Transaction"), _T("demo"), MB_OK);
	}
	goto cleanup;

abort:
	//printf("Aborting Transaction\n");
	::MessageBox(NULL, _T("Aborting Transaction"), _T("demo"), MB_OK);
	result = FwpmTransactionAbort(engineHandle);
	if (NO_ERROR == result)
	{
		//printf("Successfully Aborted Transaction\n");
		::MessageBox(NULL, _T("Successfully Aborted Transaction"), _T("demo"), MB_OK);
	}

cleanup:

	return result;
}

/*����·����ȡappId*/
DWORD MonitorAppIDFromPath(
	_In_ PCWSTR fileName,
	_Out_ FWP_BYTE_BLOB** appId)
{
	DWORD result = NO_ERROR;

	result = FwpmGetAppIdFromFileName(fileName, appId);

	return result;
}

/*���һ��Ӧ��*/
DWORD MonitorAppDoMonitoring(PCWSTR AppPath)
{
	HANDLE            monitorDevice = NULL;
	HANDLE            engineHandle = NULL;
	DWORD             result;
	MONITOR_SETTINGS  monitorSettings;
	FWPM_SESSION     session;
	FWP_BYTE_BLOB*    applicationId = NULL;

	RtlZeroMemory(&monitorSettings, sizeof(MONITOR_SETTINGS));
	RtlZeroMemory(&session, sizeof(FWPM_SESSION));

	session.displayData.name = L"Monitor Sample Session";
	session.displayData.description = L"Monitors traffic at the Stream layer.";

	// Let the Base Filtering Engine cleanup after us.
	session.flags = FWPM_SESSION_FLAG_DYNAMIC;

	//printf("Opening Filtering Engine\n");
	::MessageBox(NULL, _T("Opening Filtering Engine"), _T("demo"), MB_OK);
	result = FwpmEngineOpen(
		NULL,
		RPC_C_AUTHN_WINNT,
		NULL,
		&session,
		&engineHandle
	);

	if (NO_ERROR != result)
	{
		goto cleanup;
	}

	//printf("Successfully opened Filtering Engine\n");
	::MessageBox(NULL, _T("Successfully opened Filtering Engine"), _T("demo"), MB_OK);

	//printf("Looking up Application ID from BFE\n");
	::MessageBox(NULL, _T("Looking up Application ID from BFE"), _T("demo"), MB_OK);
	result = MonitorAppIDFromPath(AppPath, &applicationId);

	if (NO_ERROR != result)
	{
		goto cleanup;
	}

	//printf("Successfully retrieved Application ID\n");
	::MessageBox(NULL, _T("Successfully retrieved Application ID"), _T("demo"), MB_OK);
	//printf("Opening Monitor Sample Device\n");
	::MessageBox(NULL, _T("Opening Monitor Sample Device"), _T("demo"), MB_OK);
	//���ļ����߶��󣬻������  #define MONITOR_DOS_NAME   L"\\\\.\\MonitorSample"
	result = MonitorAppOpenMonitorDevice(&monitorDevice);
	if (NO_ERROR != result)
	{
		goto cleanup;
	}

	//printf("Successfully opened Monitor Device\n");
	::MessageBox(NULL, _T("Successfully opened Monitor Device"), _T("demo"), MB_OK);
	//printf("Adding Filters through the Filtering Engine\n");
	::MessageBox(NULL, _T("Adding Filters through the Filtering Engine"), _T("demo"), MB_OK);
	result = MonitorAppAddFilters(engineHandle,
		applicationId);

	if (NO_ERROR != result)
	{
		goto cleanup;
	}

	//printf("Successfully added Filters through the Filtering Engine\n");
	::MessageBox(NULL, _T("Successfully added Filters through the Filtering Engine"), _T("demo"), MB_OK);
	//printf("Enabling monitoring through the Monitor Sample Device\n");
	::MessageBox(NULL, _T("Enabling monitoring through the Monitor Sample Device"), _T("demo"), MB_OK);
	monitorSettings.monitorOperation = monitorTraffic;

	result = MonitorAppEnableMonitoring(monitorDevice,
		&monitorSettings);
	if (NO_ERROR != result)
	{
		goto cleanup;
	}

	//printf("Successfully enabled monitoring.\n");
	::MessageBox(NULL, _T("Successfully enabled monitoring."), _T("demo"), MB_OK);
	//printf("Events will be traced through WMI. Please press any key to exit and cleanup filters.\n");
	::MessageBox(NULL, _T("Events will be traced through WMI. Please press any key to exit and cleanup filters."), _T("demo"), MB_OK);

#pragma prefast(push)
#pragma prefast(disable:6031, "by design the return value of _getch() is ignored here")
	_getch();
#pragma prefast(pop)

	cleanup:

		   if (NO_ERROR != result)
		   {
			   //printf("Monitor.\tError 0x%x occurred during execution\n", result);
			   ::MessageBox(NULL, _T("Error occurred during execution"), _T("demo"), MB_OK);
		   }

		   if (monitorDevice)
		   {
			   MonitorAppCloseMonitorDevice(monitorDevice);
		   }

		   //
		   // Free the application Id that we retrieved.
		   //
		   if (applicationId)
		   {
			   FwpmFreeMemory((void**)&applicationId);
		   }

		   if (engineHandle)
		   {
			   result = FwpmEngineClose(engineHandle);
			   engineHandle = NULL;
		   }

		   return result;
}

CString path;
/*�����ť ��ȡ���̵�·��*/
void ProcessMonitor::OnBnClickedBtngetfolder()
{
	CFileDialog dlg(TRUE, NULL, NULL, NULL, NULL);
	CEdit* FilePath;
	FilePath = (CEdit*)GetDlgItem(IDC_EDIT1);	
	dlg.DoModal();
	path = dlg.GetPathName();
	FilePath->SetWindowText(path);
}

void ProcessMonitor::OnBnClickedBtnstart()
{			
	GetDlgItem(IDC_EDIT1)->GetWindowText(path);
	if (path == "")
		MessageBox(_T("选择路径为空，请选择路径"));		
	else
		MonitorAppDoMonitoring(path);
	//"C:\Users\11401\AppData\Local\Google\Chrome\Application\chrome.exe"
}


void ProcessMonitor::OnBnClickedBtnstop()
{
	
}


void ProcessMonitor::OnBnClickedBtnaddcallouts()
{
	MonitorAppAddCallouts();
}


void ProcessMonitor::OnBnClickedBtndelcallouts()
{
	MonitorAppRemoveCallouts();
}
