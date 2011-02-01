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


CBTDeviceManager::CBTDeviceManager(void)
{
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
	if(m_mapBTDevice.find(pDevice->GetAddr())==m_mapBTDevice.end()){
		m_mapBTDevice[pDevice->GetAddr()]=pDevice;
		return TRUE;
	}
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
	CBTDevice* pDevice = m_mapBTDevice[deviceAddr];
	
	pDevice->SetAddr(deviceAddr);
	pDevice->SetName(deviceName);
	pDevice->SetDeviceClass(deviceClass);
	pDevice->SetPaired(paired);
	
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
	if(m_mapBTDevice.find(deviceAddr)==m_mapBTDevice.end()){
		return FALSE;
	}
	delete m_mapBTDevice[deviceAddr];
	m_mapBTDevice.erase(deviceAddr);
	return TRUE;
}

BT_DEV_MAP CBTDeviceManager::GetDeviceMap()
{
	return m_mapBTDevice;
}