//#include "StdAfx.h"
#include "Utils.h"
#include <Windows.h>
#include <string>
#include <istream>
#include <iostream>
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

void Utils::ShowError(std::wstring lpszFunction) 
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
		(LPWSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
		(lstrlen((LPCWSTR)lpMsgBuf) + lpszFunction.length() + 40) * sizeof(wchar_t)); 
	swprintf((wchar_t *)lpDisplayBuf, 
		TEXT("%s failed with error %d: %s"), 
		lpszFunction.c_str(), dw, lpMsgBuf); 
	//MessageBox(NULL, (std::wstring)lpDisplayBuf, TEXT("Error"), MB_OK); 
	wcout<<lpDisplayBuf<<endl;

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
	if(evt.isNull==false){
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

wstring widen( const string& str )
{
	wostringstream wstm ;
	//wstm.imbue(std::locale("en_US.UTF-8"));
	wstm.imbue(std::locale(""));
	const ctype<wchar_t>& ctfacet =
	use_facet< ctype<wchar_t> >( wstm.getloc() ) ;
	for( size_t i=0 ; i<str.size() ; ++i )
	wstm << ctfacet.widen( str[i] ) ;
	return wstm.str() ;
}
       
string narrow( const wstring& str )
{
	ostringstream stm ;
	//stm.imbue(std::locale("en_US"));
	stm.imbue(std::locale(""));
	const ctype<char>& ctfacet =
	use_facet< ctype<char> >( stm.getloc() ) ;
	for( size_t i=0 ; i<str.size() ; ++i )
	stm << ctfacet.narrow( str[i], 0 ) ;
	return stm.str() ;
}
