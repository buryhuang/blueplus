#include "StdAfx.h"
#include "Utils.h"
#include <Windows.h>
#include <string>
#include <istream>
#include <sstream>
#include <iomanip>
#include "PersistentStorage.h"
using namespace std;

Utils::Utils(void)
{
}

Utils::~Utils(void)
{
}

wstring Utils::CurrentTime()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	wstring str;
	wstringstream wiss;
	wiss<<setfill(L'0');

	wiss<<st.wYear;
	wiss<<"-"<<setw(2)<<st.wMonth;
	wiss<<"-"<<setw(2)<<st.wDay;
	wiss<<" "<<setw(2)<<st.wHour;
	wiss<<":"<<setw(2)<<st.wMinute;
	wiss<<":"<<setw(2)<<st.wSecond;
	wiss<<"."<<setw(3)<<st.wMilliseconds;
	str=wiss.str();
	return str;
}

void Utils::ShowError(LPTSTR lpszFunction) 
{ 
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
	StringCchPrintf((LPTSTR)lpDisplayBuf, 
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"), 
		lpszFunction, dw, lpMsgBuf); 
	//MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 
	wcout<<(LPCTSTR)lpDisplayBuf<<endl;

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

void Utils::LogEvent(wstring source_id, wstring source_desc, long long msg_id, wstring msg)
{
	wstringstream wss;
	wss<<L"'"<<source_id<<L"'";
	wss<<L",'"+source_desc+L"'";
	wss<<","<<msg_id;
	wss<< ",'"<<msg<<L"'";
	PERSISTENT_STORAGE_PTR->InsertTimedValues(L"events",wss.str());
}

#ifdef UNITTEST
#include "unittest_config.h"
#include "gtest/gtest.h"

TEST(UtilsTest,Logging)
{
	Utils::LogEvent(L"UTEST_SRCID",L"UTEST_SRCDESC",999999,L"UTEST_EVTDESC");

	CEvent evt = PERSISTENT_STORAGE_PTR->GetLastRecord();
	if(evt.isNull==FALSE){
		wcout<<evt.timestamp<<"\t";
		wcout<<evt.source_id<<"\t";
		wcout<<evt.source_desc<<"\t";
		wcout<<evt.event_id<<"\t";
		wcout<<evt.event_desc<<"\t";
		wcout<<endl;
		ASSERT_TRUE(evt.source_id==L"UTEST_SRCID");
		ASSERT_TRUE(evt.source_desc==L"UTEST_SRCDESC");
		ASSERT_TRUE(evt.event_id==999999);
		ASSERT_TRUE(evt.event_desc==L"UTEST_EVTDESC");
	}
}


#endif