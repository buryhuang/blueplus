/** \addtogroup application
 *  @{
 */

#pragma once

#include "ManagedThread.h"
#include "BlueTooth.h"
#include "BlueToothSocket.h"
#include <map>
using namespace std;

#define DEF_BTDEV_MGR CBTDeviceManager::GetInstance(L"BT Device Manager")

class CDevMgrBTHandlerThread:public CSocketHandler, public CManagedThread
{
public:

	CDevMgrBTHandlerThread::CDevMgrBTHandlerThread(wstring name,BTH_ADDR addr)
		:CManagedThread(name),m_addrBth(addr){}
	virtual void OnAccept(SOCKET s){}
	virtual void OnReceive(SOCKET s, BYTEBUFFER buff);
	virtual void OnConnect(){};
	virtual void OnClose(){};
	virtual int Run();
	wstring GetStatusString(){return m_pSocket->GetStatusString();}
private:
	BTH_ADDR m_addrBth;
	CBlueToothSocket * m_pSocket;
};

class CBTDevice
{
public:
	CBTDevice(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired);
	CDevMgrBTHandlerThread* m_pSockHandler;
	BTH_ADDR m_addrBth;
	bool m_bPaired;
	wstring m_deviceName;
	int m_iDeviceClass;
};

typedef map<BTH_ADDR, CBTDevice*> BT_DEV_MAP;


class CBTDeviceManager: public CManagedThread
{
public:
	CBTDeviceManager(void);
	CBTDeviceManager(wstring name):CManagedThread(name){};
	virtual ~CBTDeviceManager(void);
	virtual int Run();

	virtual bool RegisterDevice(CBTDevice* pDevice);
	virtual bool RegisterDevice(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired);
	virtual bool UnregisterDevice(CBTDevice* pDevice);
	virtual bool UnregisterDevice(BTH_ADDR deviceAddr);
	virtual bool UpdateDevice(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired);
	virtual bool UpdateDevice(CBTDevice* pDevice);

	virtual BT_DEV_MAP GetDeviceMap();
	void ListDevices();

	static CBTDeviceManager* GetInstance(wstring name){
		if(m_instance==NULL){
			m_instance = new CBTDeviceManager(name);
		}
		return m_instance;
	}

protected:
	BT_DEV_MAP m_mapBTDevice;
	static CBTDeviceManager* m_instance;

	HANDLE m_hMutex;
};

/** @}*/
