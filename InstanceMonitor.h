/** \addtogroup application
 *  @{
 */

#pragma once

#include "App.h"
#include "LocalMessage.h"
#include "ManagedThread.h"
#include <vector>
using namespace std;

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
	vector<CManagedThread*> m_listThreads;
	static CInstanceMonitor* m_instance;
};

/** @}*/
