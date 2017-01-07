
// stdafx.cpp : 只包括标准包含文件的源文件
// PFirewall.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

typedef struct Rule {
	CString sourceIP;
	CString destinIP;
	int type;
	int sourcePort;
	int destinPort;
	bool inout;
	bool operation;
}Rule;






