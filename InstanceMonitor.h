#pragma once

#include "App.h"
#include "BTDeviceManager.h"
#include "LocalMessage.h"

#define INST_MON_PTR (CInstanceMonitor::GetInstance());

class CInstanceMonitor:public CApp
{
public:
	CInstanceMonitor(void);

	static CInstanceMonitor * GetInstance()
	{
		if(m_instance == NULL){
			return new CInstanceMonitor();
		}
		return m_instance;
	}
	virtual ~CInstanceMonitor(void);

	void AppStart();
	void GraceShutdown();
private:
	static CInstanceMonitor* m_instance;

	CBTDeviceManager* m_BTDeviceManager;
};
