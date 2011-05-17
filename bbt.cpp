// bbt.cpp : 定义控制台应用程序的入口点。
//
//#include "stdafx.h"
#include "InstanceMonitor.h"

#ifdef UNITTEST
#include "gtest/gtest.h"
#endif

using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _UNICODE
#define _tmain wmain
#else
#define _tmain main
#endif



// 唯一的应用程序对象

//CWinApp theApp;
CInstanceMonitor theApp;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
#ifdef UNITTEST
  std::cout << "Running main() from gtest_main.cc\n";

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();	
#else
	theApp.AppStart();
#endif
	return 0;
}
