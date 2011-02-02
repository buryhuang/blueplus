#ifndef _UTILS_H
#define _UTILS_H

#pragma once
#include <string>
#include <strsafe.h>
using namespace std;

class Utils
{
public:
	Utils(void);
	~Utils(void);
	static void ShowError(LPTSTR lpszFunction);

	static wstring CurrentTime();
	static void LogEvent(wstring source_id, wstring source_desc, long long msg_id, wstring msg);
};

#define LOGSVREVENT(msg_id, msg) Utils::LogEvent(L"Localhost",L"Server",(msg_id),(msg))
#define LOGBTEVENT(id,msg_id,msg) Utils::LogEvent((id),L"Bluetooth",(msg_id),(msg))
#define LOGFDEVENT(id,msg_id,msg) Utils::LogEvent((id),L"FlashDrive",(msg_id),(msg))

#endif
