#include "StdAfx.h"
#include "InstanceMonitor.h"
#include <iostream>
using namespace std;


CInstanceMonitor::CInstanceMonitor(void):
m_BTDeviceManager(NULL)
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

	if(m_BTDeviceManager!=NULL){
		delete m_BTDeviceManager;
	}
}

void CInstanceMonitor::AppStart()
{

	//Start service
	m_BTDeviceManager=new CBTDeviceManager(L"BT Device Manager");
	m_BTDeviceManager->Start();

	wcout<<_L(IDS_DEBUG_STARTED_SERVICE)<<m_BTDeviceManager->GetName()<<endl;


	//Main loop: Monitor workers and keep alive.
	while(true){
		STDOUT<<_L(IDS_DEBUG_CHECKING_SERVICE)<<m_BTDeviceManager->GetName()<<endl;

		if(!m_BTDeviceManager->IsAlive()){
			STDOUT<<_T("Retarting ")<<m_BTDeviceManager->GetName()<<endl;
			m_BTDeviceManager->Restart();
		}

		Sleep(2000);
	}
}

void CInstanceMonitor::GraceShutdown()
{
	if(m_BTDeviceManager!=NULL){
		m_BTDeviceManager->GraceShutdown();
	}
}