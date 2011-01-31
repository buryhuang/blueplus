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
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

void Utils::LogEvent(wstring msg)
{
	PERSISTENT_STORAGE_PTR->InsertTimedValues(L"events", L"'"+msg+L"'");
}