#pragma once

#include "stdafx.h"
#include "ManagedObject.h"

class CBTDeviceManager: public CManagedThread
{
public:
	CBTDeviceManager(void);
	CBTDeviceManager(wstring name):CManagedThread(name){};
	virtual ~CBTDeviceManager(void);
	virtual int Run();
};
