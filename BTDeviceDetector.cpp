#include "BTDeviceDetector.h"
#include "BTHandler.h"

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
	BTHandler mbt;

	//Utils::ShowError(TEXT("_tmain"));

	wprintf(L"BT status %d\n",mbt.InitializationStatus());

	mbt.RunDeviceInquiry(10);

	return 0;
}