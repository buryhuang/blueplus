/** \addtogroup application
 *  @{
 */

#pragma once

#include "ManagedThread.h"

#define DEF_BTDETECTOR CBTDeviceDetector::GetInstance(L"BT Device Detector")

class CBTDeviceDetector :
	public CManagedThread
{
public:
	CBTDeviceDetector(void);
	CBTDeviceDetector(wstring name):CManagedThread(name){};
	virtual ~CBTDeviceDetector(void);
	virtual int Run();

	static CBTDeviceDetector* GetInstance(wstring name){
		if(m_instance==NULL){
			m_instance = new CBTDeviceDetector(name);
		}
		return m_instance;
	}

protected:
	static CBTDeviceDetector* m_instance;

};

/** @}*/
