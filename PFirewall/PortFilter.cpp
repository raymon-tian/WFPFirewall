// PortFilter.cpp : 实现文件
//

#include "stdafx.h"
#include "PFirewall.h"
#include "PortFilter.h"
#include "afxdialogex.h"


// PortFilter 对话框

IMPLEMENT_DYNAMIC(PortFilter, CDialogEx)

PortFilter::PortFilter(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PortFilter, pParent)
{

}

PortFilter::~PortFilter()
{
}

void PortFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PortFilter, CDialogEx)
END_MESSAGE_MAP()


// PortFilter 消息处理程序
