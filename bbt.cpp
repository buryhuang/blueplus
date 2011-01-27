// bbt.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "BlueTooth.h"
#include <iostream>

using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

class MyBT: public CBlueTooth
{
	void OnDeviceDiscovered(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, BOOL paired)
	{
		wcout<<deviceAddr<<" - "<<deviceName<<endl;
	}
};


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		MyBT mbt;

		//Utils::ShowError(TEXT("_tmain"));

		wprintf(L"BT status %d\n",mbt.InitializationStatus());
		while(true){
		mbt.RunDeviceInquiry(10);
		}
	}

	return nRetCode;
}
