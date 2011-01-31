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
	static void LogEvent(wstring msg);
};

#endif
