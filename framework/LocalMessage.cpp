/** \addtogroup framework
 *  @{
 */

#include "LocalMessage.h"

std::wstring g_str;

std::wstring LoadStringFromResource(__in UINT stringID,__in_opt HINSTANCE instance)

{

    WCHAR * pBuf = NULL;

    int len = LoadStringW(
        instance,
        stringID,
        reinterpret_cast< LPWSTR >( &pBuf ),
        0 );

    if( len )
		return std::wstring( pBuf, len );
    else
		return std::wstring();
}

/** @}*/
