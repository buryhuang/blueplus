#include "config.h"
#include "InstanceMonitor.h"

#include "BTDeviceDetector.h"
#include "BTDeviceManager.h"
#include "BTServer.h"

#include "PersistentStorage.h"
#include "Utils.h"

#include <iostream>
using namespace std;


CInstanceMonitor::CInstanceMonitor(void)
{
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
	}
}

CInstanceMonitor::~CInstanceMonitor(void)
{
	GraceShutdown();
	vector<CManagedThread*>::iterator vi=m_listThreads.begin();
	for(;vi!=m_listThreads.end();vi++){
		if((*vi)!=NULL){
			delete (*vi);
		}
	}
}

void CInstanceMonitor::AppStart()
{
	LOGSVREVENT(0,L"Starting App");

	//Register services
	CBTDeviceDetector* pBTDeviceDetector=new CBTDeviceDetector(L"BT Device Detector");
	this->m_listThreads.push_back(pBTDeviceDetector);

	this->m_listThreads.push_back(DEF_BTDEV_MGR);

	this->m_listThreads.push_back(DEF_BTSERVER);

	//Start services
	vector<CManagedThread*>::iterator vi=m_listThreads.begin();
	for(;vi!=m_listThreads.end();vi++){
		(*vi)->Start();
		wcout<<_L(IDS_DEBUG_STARTED_SERVICE)<<(*vi)->GetName()<<endl;
	}


	//Main loop: Monitor workers and keep alive.
	while(true){
		vector<CManagedThread*>::iterator vi=m_listThreads.begin();
		for(;vi!=m_listThreads.end();vi++){
			STDOUT<<_L(IDS_DEBUG_CHECKING_SERVICE)<<(*vi)->GetName()<<endl;
			if(!(*vi)->IsAlive()){
				STDOUT<<_T("Retarting ")<<(*vi)->GetName()<<endl;
				(*vi)->Restart();
			}
		}

		Sleep(MON_THREAD_CHECK_INTERVAL_MS);
	}

}

void CInstanceMonitor::GraceShutdown()
{
	vector<CManagedThread*>::iterator vi=m_listThreads.begin();
	for(;vi!=m_listThreads.end();vi++){
		if((*vi)!=NULL){
			(*vi)->GraceShutdown();
		}
		wcout<<_L(IDS_DEBUG_STARTED_SERVICE)<<(*vi)->GetName()<<endl;
	}

}