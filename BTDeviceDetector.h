/** \addtogroup application
 *  @{
 */

#pragma once

#include "ManagedThread.h"

class CBTDeviceDetector :
	public CManagedThread
{
public:
	CBTDeviceDetector(void);
	CBTDeviceDetector(wstring name):CManagedThread(name){};
	virtual ~CBTDeviceDetector(void);
	virtual int Run();
};

/** @}*/
