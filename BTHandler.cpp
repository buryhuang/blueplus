/** \addtogroup application
 *  @{
 */

#include "BTHandler.h"
#include "BTDeviceManager.h"

#include <iostream>

CBTHandler::CBTHandler(void)
{
}

CBTHandler::~CBTHandler(void)
{
}

void CBTHandler::OnDeviceDiscovered(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired)
{
	wcout<<hex<<deviceAddr<<" - "<<deviceName<<endl;
	if(DEF_BTDEV_MGR->RegisterDevice(deviceAddr, deviceClass, deviceName, paired)!=TRUE){
		wcout<<L"Registering failed"<<endl;
		if(DEF_BTDEV_MGR->UpdateDevice(deviceAddr, deviceClass, deviceName, paired)!=TRUE){
			wcout<<L"Updating failed"<<endl;
		}
	}

	if(RunSearchServices(deviceAddr)==FALSE){
		Utils::ShowError(L"CBTHandler::OnDeviceDiscovered");
	}
}

void CBTHandler::OnServiceDiscovered(BTH_ADDR deviceAddr, vector<ServiceRecord> serviceList)
{
	for(vector<ServiceRecord>::iterator vi=serviceList.begin();vi!=serviceList.end();vi++){
		wcout<<vi->serviceInstanceName
			<<L"-"<<vi->comment
			<<L"-"<<vi->sockaddrBth.btAddr
			<<L"-"<<vi->sockaddrBth.port
			<<endl;
	}
	DEF_BTDEV_MGR->UpdateServices(deviceAddr, serviceList);
}

/** @}*/
