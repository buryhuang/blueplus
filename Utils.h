#ifndef _UTILS_H
#define _UTILS_H

#pragma once
#include <string>
#include <sstream>
//#include <strsafe.h>
using namespace std;

wstring widen( const string& str );
string narrow( const wstring& str );


class Utils
{
public:
	Utils(void);
	~Utils(void);
	static void ShowError(std::wstring lpszFunction);

	static wstring CurrentTime();
	static void LogEvent(wstring source_id, wstring source_desc, long long msg_id, wstring msg);
};

#define LOGSVREVENT(msg_id, msg) Utils::LogEvent(L"Localhost",L"Server",(msg_id),(msg))
#define LOGBTEVENT(id,msg_id,msg) Utils::LogEvent((id),L"Bluetooth",(msg_id),(msg))
#define LOGFDEVENT(id,msg_id,msg) Utils::LogEvent((id),L"FlashDrive",(msg_id),(msg))

//#define _T(str) ((wchar_t *)(widen(str).c_str()))

#endif
