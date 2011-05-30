/** \addtogroup application
 *  @{
 */

#include "config.h"
#include "BTDeviceDetector.h"
#include "BTDeviceManager.h"
#include <iostream>
using namespace std;


CBTDeviceDetector* CBTDeviceDetector::m_instance=NULL;

CBTDeviceDetector::CBTDeviceDetector(void)
{
}

CBTDeviceDetector::~CBTDeviceDetector(void)
{
}

int CBTDeviceDetector::Run()
{

	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误

	//Utils::ShowError(TEXT("_tmain"));
	while(true){

		//Be sure to use new Bluetooth class to correctly init WSA
		bool btInited = DEF_BTDEVICE->InitializationStatus();
		wprintf(L"BlueTooth initialization status: %d\n",btInited);
		if(btInited) {
			DEF_BTDEVICE->RegisterHandler(DEF_BTDEV_HANDLER);
			DEF_BTDEVICE->RunDeviceInquiry(BT_DETECT_DURATION_SECONDS);
		} else {
			wcout<<L"Wait until BlueTooth is available"<<endl;
		}

		Sleep(BT_DETECT_INTERVAL_MS);
	}

	return 0;
}

/** @}*/
