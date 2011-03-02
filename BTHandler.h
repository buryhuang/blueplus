/** \addtogroup application
 *  @{
 */

#ifndef _BTHANDLER_H_
#define _BTHANDLER_H_

#include "BlueTooth.h"

class CBTDeviceMgrBTHandler :
	public CBTHandler
{
public:
	CBTDeviceMgrBTHandler(void);
	virtual ~CBTDeviceMgrBTHandler(void);

	virtual void OnDeviceDiscovered(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired);
	virtual void OnServiceDiscovered(BTH_ADDR deviceAddr, vector<ServiceRecord>);

};

#endif


/** @}*/
