#include "BTDeviceManager.h"
#include <iostream>
using namespace std;

CBTDeviceManager* CBTDeviceManager::m_instance=NULL;

CBTDevice::CBTDevice(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired):
	CManagedObject(deviceName),
	m_addrBth(deviceAddr),
	m_iDeviceClass(deviceClass),
	m_bPaired(paired)
{
}


CBTDeviceManager::CBTDeviceManager(void):m_hSem(NULL)
{
	SECURITY_ATTRIBUTES attr;
	attr.bInheritHandle=TRUE;
	attr.lpSecurityDescriptor=NULL;
	attr.nLength=sizeof(SECURITY_ATTRIBUTES);

	m_hSem = CreateSemaphore(&attr,0,1000,NULL);
}

CBTDeviceManager::~CBTDeviceManager(void)
{
	BT_DEV_MAP::iterator mit = m_mapBTDevice.begin();
	for(;mit!=m_mapBTDevice.end();mit++){
		if(mit->second != NULL){
			delete mit->second;
		}
	}
	m_mapBTDevice.clear();

	if(m_hSem!=NULL){
		CloseHandle(m_hSem);
	}
}

int CBTDeviceManager::Run()
{
	while(true){
		wcout<<L"***Device list:***"<<endl;
		BT_DEV_MAP::iterator mit = m_mapBTDevice.begin();
		int cnt=1;
		for(;mit!=m_mapBTDevice.end();mit++){
			if(mit->second != NULL){
				wcout<<cnt<<". "<<hex<<mit->second->GetAddr()<<L" - "<<mit->second->GetName()<<endl;
				cnt++;
			}
		}
		wcout<<L"***End of list***"<<endl<<endl;
		Sleep(5000);
	}
	return 0;
}

bool CBTDeviceManager::RegisterDevice(CBTDevice* pDevice)
{
	if(pDevice == NULL){
		return FALSE;
	}
	WaitForSingleObject(m_hSem,INFINITE);
	if(m_mapBTDevice.find(pDevice->GetAddr())==m_mapBTDevice.end()){
		m_mapBTDevice[pDevice->GetAddr()]=pDevice;
		return TRUE;
	}
	ReleaseSemaphore(m_hSem,1,NULL);
	delete pDevice;
	return FALSE;
}

bool CBTDeviceManager::RegisterDevice(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired)
{
	if(m_mapBTDevice.find(deviceAddr)==m_mapBTDevice.end()){
		CBTDevice* pDevice = new CBTDevice(deviceAddr, deviceClass, deviceName, paired);
		return RegisterDevice(pDevice);
	}
	return FALSE;
}

bool CBTDeviceManager::UpdateDevice(CBTDevice* pDevice)
{
	if(m_mapBTDevice.find(pDevice->GetAddr())==m_mapBTDevice.end()){
		return FALSE;
	}
	
	return UpdateDevice(
			pDevice->GetAddr()
			,pDevice->GetDeviceClass()
			,pDevice->GetName()
			,pDevice->IsPaired()
		);
}

bool CBTDeviceManager::UpdateDevice(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired)
{
	if(m_mapBTDevice.find(deviceAddr)==m_mapBTDevice.end()){
		return FALSE;
	}

	WaitForSingleObject(m_hSem,INFINITE);
	CBTDevice* pDevice = m_mapBTDevice[deviceAddr];
	
	pDevice->SetAddr(deviceAddr);
	pDevice->SetName(deviceName);
	pDevice->SetDeviceClass(deviceClass);
	pDevice->SetPaired(paired);

	ReleaseSemaphore(m_hSem,1,NULL);
	
	return TRUE;
}

bool CBTDeviceManager::UnregisterDevice(CBTDevice* pDevice)
{
	if(pDevice == NULL){
		return FALSE;
	}
	BTH_ADDR deviceAddr = pDevice->GetAddr();
	if(m_mapBTDevice.find(deviceAddr)==m_mapBTDevice.end()){
		return FALSE;
	}
	return UnregisterDevice(pDevice->GetAddr());
}
bool CBTDeviceManager::UnregisterDevice(BTH_ADDR deviceAddr)
{
	WaitForSingleObject(m_hSem,INFINITE);
	if(m_mapBTDevice.find(deviceAddr)==m_mapBTDevice.end()){
		return FALSE;
	}
	delete m_mapBTDevice[deviceAddr];
	m_mapBTDevice.erase(deviceAddr);

	ReleaseSemaphore(m_hSem,1,NULL);

	return TRUE;
}

BT_DEV_MAP CBTDeviceManager::GetDeviceMap()
{
	return m_mapBTDevice;
}