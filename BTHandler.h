/** \addtogroup application
 *  @{
 */

#ifndef _BTHANDLER_H_
#define _BTHANDLER_H_

#include "BlueTooth.h"

class CBTHandler :
	public CBlueTooth
{
public:
	CBTHandler(void);
	virtual ~CBTHandler(void);

	virtual void OnDeviceDiscovered(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired);
	virtual void OnServiceDiscovered(BTH_ADDR deviceAddr, vector<ServiceRecord>);

};

#endif


/** @}*/
