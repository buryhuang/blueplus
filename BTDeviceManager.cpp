/** \addtogroup application
 *  @{
 */

#include "config.h"
#include "BTDeviceManager.h"
#include <iostream>

using namespace std;

CBTDeviceManager* CBTDeviceManager::m_instance=NULL;

void CDevMgrBTHandlerThread::OnReceive(SOCKET s, BYTEBUFFER buff)
{
	cout<<"Received: "<<buff<<endl;
};
int CDevMgrBTHandlerThread::Run()
{
	m_pSocket = new CBlueToothSocket();
	m_pSocket->RegisterHandler(this);
	m_pSocket->Create(TRUE,FALSE);
	m_pSocket->SetPasskey(L"0000");
	m_pSocket->Connect(m_sockaddrBth.btAddr,m_sockaddrBth.port,10);

	//Thread main loop
	while(m_pSocket->Recveive());

	return 0;
}

CBTDevice::CBTDevice(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired):
	m_deviceName(deviceName),
	m_addrBth(deviceAddr),
	m_iDeviceClass(deviceClass),
	m_bPaired(paired),
	m_pSockHandler(NULL)
{
}

CBTDeviceManager::CBTDeviceManager(void):m_hMutex(NULL)
{
	SECURITY_ATTRIBUTES attr;
	attr.bInheritHandle=TRUE;
	attr.lpSecurityDescriptor=NULL;
	attr.nLength=sizeof(SECURITY_ATTRIBUTES);

	m_hMutex = CreateMutex(NULL,FALSE,NULL);
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

	if(m_hMutex!=NULL){
		CloseHandle(m_hMutex);
	}
}

void CBTDeviceManager::ListDevices()
{
	WaitForSingleObject(m_hMutex,INFINITE);
	wcout<<L"***Device list:***"<<endl;
	wcout<<L"Size of list is: "<<dec<<m_mapBTDevice.size()<<endl;
	BT_DEV_MAP::iterator mit = m_mapBTDevice.begin();
	int cnt=1;
	for(;mit!=m_mapBTDevice.end();mit++){
		if(mit->second != NULL){
			CBTDevice* pDev = mit->second;
			wcout<<cnt<<". "<<hex<<pDev->m_addrBth<<L" - "<<pDev->m_iDeviceClass<<L" - "<<pDev->m_deviceName<<endl;
			cnt++;
		}
	}
	wcout<<L"***End of list***"<<endl<<endl;
	ReleaseMutex(m_hMutex);
}

int CBTDeviceManager::Run()
{
	while(true){
		WaitForSingleObject(m_hMutex,INFINITE);
		wcout<<L"***Device list:***"<<endl;
		wcout<<L"Size of list is: "<<dec<<m_mapBTDevice.size()<<endl;
		BT_DEV_MAP::iterator mit = m_mapBTDevice.begin();
		int cnt=1;
		for(;mit!=m_mapBTDevice.end();mit++){
			if(mit->second != NULL){
				CBTDevice* pDev = mit->second;
				wcout<<cnt<<". "<<hex<<pDev->m_addrBth<<L" - "<<pDev->m_iDeviceClass<<L" - "<<pDev->m_deviceName;
				cnt++;

				if(pDev->m_pSockHandler == NULL){
					if(pDev->m_listService.size()>0){
						pDev->m_pSockHandler = new CDevMgrBTHandlerThread(L"DevManager BT Handler", pDev->m_listService[0].sockaddrBth);
						pDev->m_pSockHandler->Start();
					}
				}

				else if(pDev->m_pSockHandler->IsAlive()){
					wcout<<L" - "<<pDev->m_pSockHandler->GetStatusString() <<endl;
				}else{
					wcout<<L" - Not active"<<endl;
				}

				wcout<<endl;
			}
		}
		wcout<<L"***End of list***"<<endl<<endl;
		ReleaseMutex(m_hMutex);

		Sleep(BT_DEV_MGR_ROUTINE_INTERVAL_MS);
	}
	return 0;
}

bool CBTDeviceManager::UpdateServices(BTH_ADDR deviceAddr, vector<ServiceRecord> serviceList)
{
	if(m_mapBTDevice.find(deviceAddr)==m_mapBTDevice.end()){
		return FALSE;
	}

	WaitForSingleObject(m_hMutex,INFINITE);
	CBTDevice* pDevice = m_mapBTDevice[deviceAddr];
	
	pDevice->m_listService = serviceList;

	ReleaseMutex(m_hMutex);
	
	return TRUE;
}


bool CBTDeviceManager::RegisterDevice(CBTDevice* pDevice)
{
	if(pDevice == NULL){
		return FALSE;
	}
	WaitForSingleObject(m_hMutex,INFINITE);
	if(m_mapBTDevice.find(pDevice->m_addrBth)==m_mapBTDevice.end()){
		m_mapBTDevice[pDevice->m_addrBth]=pDevice;
		return TRUE;
	}
	ReleaseMutex(m_hMutex);
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
	if(m_mapBTDevice.find(pDevice->m_addrBth)==m_mapBTDevice.end()){
		return FALSE;
	}
	
	return UpdateDevice(
			pDevice->m_addrBth
			,pDevice->m_iDeviceClass
			,pDevice->m_deviceName
			,pDevice->m_bPaired
		);
}

bool CBTDeviceManager::UpdateDevice(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired)
{
	if(m_mapBTDevice.find(deviceAddr)==m_mapBTDevice.end()){
		return FALSE;
	}

	WaitForSingleObject(m_hMutex,INFINITE);
	CBTDevice* pDevice = m_mapBTDevice[deviceAddr];
	
	pDevice->m_addrBth=deviceAddr;
	pDevice->m_deviceName=deviceName;
	pDevice->m_iDeviceClass=deviceClass;
	pDevice->m_bPaired=paired;

	ReleaseMutex(m_hMutex);
	
	return TRUE;
}

bool CBTDeviceManager::UnregisterDevice(CBTDevice* pDevice)
{
	if(pDevice == NULL){
		return FALSE;
	}
	BTH_ADDR deviceAddr = pDevice->m_addrBth;
	if(m_mapBTDevice.find(deviceAddr)==m_mapBTDevice.end()){
		return FALSE;
	}
	return UnregisterDevice(pDevice->m_addrBth);
}
bool CBTDeviceManager::UnregisterDevice(BTH_ADDR deviceAddr)
{
	WaitForSingleObject(m_hMutex,INFINITE);
	if(m_mapBTDevice.find(deviceAddr)==m_mapBTDevice.end()){
		return FALSE;
	}
	delete m_mapBTDevice[deviceAddr];
	m_mapBTDevice.erase(deviceAddr);

	ReleaseMutex(m_hMutex);

	return TRUE;
}

BT_DEV_MAP CBTDeviceManager::GetDeviceMap()
{
	return m_mapBTDevice;
}

#ifdef UNITTEST

#include "unittest_config.h"
#include "gtest/gtest.h"

//Unit Test
TEST(BTDeviceManagerTest,Init)
{
	ASSERT_TRUE(DEF_BTDEV_MGR!=NULL);
}

//Normal Registration
TEST(BTDeviceManagerTest,RegisterDevice)
{
	ASSERT_FALSE(DEF_BTDEV_MGR->RegisterDevice((CBTDevice*)NULL));

	ASSERT_TRUE(DEF_BTDEV_MGR->RegisterDevice(0x0001,0,L"DummyBT1",FALSE));
	ASSERT_FALSE(DEF_BTDEV_MGR->RegisterDevice(0x0001,0,L"DummyBT1",FALSE));
	ASSERT_TRUE(DEF_BTDEV_MGR->UpdateDevice(0x0001,0,L"DummyBT1_Updated",FALSE));
	ASSERT_TRUE(DEF_BTDEV_MGR->GetDeviceMap().size() == 1);
}

//Normal Unregistration
TEST(BTDeviceManagerTest,UnregisterDevice)
{
	ASSERT_FALSE(DEF_BTDEV_MGR->UnregisterDevice((CBTDevice*)NULL));

	ASSERT_TRUE(DEF_BTDEV_MGR->UnregisterDevice(0x0001));
	ASSERT_FALSE(DEF_BTDEV_MGR->UnregisterDevice(0x0001));
	ASSERT_TRUE(DEF_BTDEV_MGR->GetDeviceMap().size() == 0);
}

//Register unique address simutenously
DWORD WINAPI RegisterDeviceThreadFunc( LPVOID lpParam )
{
	Sleep(rand()%UNITTEST_DEVMGR_THREAD_DELAY_MS);
	//wcout<<"Registering "<<lpParam<<endl;
	EXPECT_EQ(DEF_BTDEV_MGR->RegisterDevice((BTH_ADDR)lpParam,0,L"DummyMTBT",FALSE),true);
	return 0;
}
TEST(BTDeviceManagerTest,MTRegisterDevice)
{
	DWORD   dwThreadIds[UNITTEST_DEVMGR_MAX_THREADS];
	HANDLE	hThreads[UNITTEST_DEVMGR_MAX_THREADS];

	for(int cnt=0;cnt<UNITTEST_DEVMGR_MAX_THREADS;cnt++){
		hThreads[cnt] = CreateThread( 
			NULL,                   // default security attributes
			0,                      // use default stack size  
			RegisterDeviceThreadFunc,       // thread function name
			(LPVOID)(BTH_ADDR)cnt,          // argument to thread function 
			CREATE_SUSPENDED,     // use default creation flags 
			&dwThreadIds[cnt]);   // returns the thread identifier 
	}
	for(int cnt=0;cnt<UNITTEST_DEVMGR_MAX_THREADS;cnt++){
		ResumeThread(hThreads[cnt]);
	}
	//WaitForMultipleObjects(UNITTEST_DEVMGR_MAX_THREADS, hThreads, TRUE, INFINITE);
	for(int cnt=0;cnt<UNITTEST_DEVMGR_MAX_THREADS;cnt++){
		WaitForSingleObject(hThreads[cnt],INFINITE);
	}

	//DEF_BTDEV_MGR->ListDevices();//Check result, not starting thread
	ASSERT_TRUE(DEF_BTDEV_MGR->GetDeviceMap().size() == UNITTEST_DEVMGR_MAX_THREADS);
}


//Unregister duplicated addresses simutenously
//The number of UNITTEST_DEVMGR_MAX_CONFLICTS threads are unregisterring the same address
DWORD WINAPI UnregisterDeviceThreadFunc( LPVOID lpParam )
{
	Sleep(rand()%UNITTEST_DEVMGR_THREAD_DELAY_MS);
	DEF_BTDEV_MGR->UnregisterDevice((BTH_ADDR)lpParam);
	return 0;
}
TEST(BTDeviceManagerTest,MTUnregisterDevice)
{
	DWORD   dwThreadIds[UNITTEST_DEVMGR_MAX_THREADS*UNITTEST_DEVMGR_MAX_CONFLICTS];
	HANDLE	hThreads[UNITTEST_DEVMGR_MAX_THREADS*UNITTEST_DEVMGR_MAX_CONFLICTS];

	for(int cnt=0;cnt<UNITTEST_DEVMGR_MAX_THREADS*UNITTEST_DEVMGR_MAX_CONFLICTS;cnt++){
		hThreads[cnt] = CreateThread( 
			NULL,                   // default security attributes
			0,                      // use default stack size  
			UnregisterDeviceThreadFunc,       // thread function name
			(LPVOID)(BTH_ADDR)(cnt%UNITTEST_DEVMGR_MAX_THREADS),          // argument to thread function 
			CREATE_SUSPENDED,     // use default creation flags 
			&dwThreadIds[cnt]);   // returns the thread identifier 
	}
	for(int cnt=0;cnt<UNITTEST_DEVMGR_MAX_THREADS*UNITTEST_DEVMGR_MAX_CONFLICTS;cnt++){
		ResumeThread(hThreads[cnt]);
	}
	//WaitForMultipleObjects(UNITTEST_DEVMGR_MAX_THREADS, hThreads, TRUE, INFINITE);
	for(int cnt=0;cnt<UNITTEST_DEVMGR_MAX_THREADS*UNITTEST_DEVMGR_MAX_CONFLICTS;cnt++){
		WaitForSingleObject(hThreads[cnt],INFINITE);
	}

	DEF_BTDEV_MGR->ListDevices();//Check result, not starting thread
	ASSERT_TRUE(DEF_BTDEV_MGR->GetDeviceMap().size() == 0);
}

#endif


/** @}*/
