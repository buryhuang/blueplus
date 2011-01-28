// bbt.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "InstanceMonitor.h"

using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

//CWinApp theApp;
CInstanceMonitor theApp;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	theApp.AppStart();
	return 0;
}
