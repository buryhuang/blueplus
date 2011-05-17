/** \addtogroup framework
 *  @{
 */

#pragma once

//#include "afx.h"
#include "Windows.h"
#include "..\\Resource.h"
#include <iostream>
using namespace std;

extern std::wstring g_str;
std::wstring LoadStringFromResource(__in UINT stringID,__in_opt HINSTANCE instance = NULL );

#define _L(id) (LoadStringFromResource(id))

#define STDOUT wcout

/** @}*/
