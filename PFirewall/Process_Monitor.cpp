// Process_Monitor.cpp : 实现文件
//

#include "stdafx.h"
#include "PFirewall.h"
#include "Process_Monitor.h"
#include "afxdialogex.h"


// Process_Monitor 对话框

IMPLEMENT_DYNAMIC(Process_Monitor, CDialogEx)

Process_Monitor::Process_Monitor(CWnd* pParent /*=NULL*/)
	: CDialogEx(Process_Monitor, pParent)
{

}

Process_Monitor::~Process_Monitor()
{
}

void Process_Monitor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Process_Monitor, CDialogEx)
END_MESSAGE_MAP()


// Process_Monitor 消息处理程序
