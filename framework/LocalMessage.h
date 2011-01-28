#pragma once

#include "afx.h"
#include "..\\Resource.h"
#include <iostream>
using namespace std;

extern CString g_str;

#define _L(id) (g_str.LoadString(id),g_str.GetString())

#define STDOUT wcout
