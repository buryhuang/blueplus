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
#define DEF_BTDEV_HANDLER CBTDeviceMgrBTHandler::GetInstance()

class CBTDeviceMgrBTHandler :
	public CBTHandler
{
public:
	CBTDeviceMgrBTHandler(void);
	virtual ~CBTDeviceMgrBTHandler(void);

	virtual void OnDeviceDiscovered(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired);
	virtual void OnServiceDiscovered(BTH_ADDR deviceAddr, vector<ServiceRecord>);

	static CBTDeviceMgrBTHandler* GetInstance(){
		if(m_instance==NULL){
			m_instance = new CBTDeviceMgrBTHandler();
		}
		return m_instance;
	}
protected:
	static CBTDeviceMgrBTHandler* m_instance;
};

class CDevMgrBTHandlerThread:public CSocketHandler, public CManagedThread
{
public:

	CDevMgrBTHandlerThread::CDevMgrBTHandlerThread(wstring name,SOCKADDR_BTH sockaddr)
		:CManagedThread(name),m_sockaddrBth(sockaddr){}
	virtual void OnAccept(SOCKET s){}
	virtual void OnReceive(SOCKET s, BYTEBUFFER buff);
	virtual void OnConnect();
	virtual void OnClose(){};
	virtual int Run();
	wstring GetStatusString(){return m_pSocket->GetStatusString();}
	CBlueToothSocket::CONNECT_STATUS_T GetStatus(){return m_pSocket->GetStatus();};
private:
	SOCKADDR_BTH m_sockaddrBth;
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
	vector<ServiceRecord> m_listService;
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

	virtual bool UpdateServices(BTH_ADDR deviceAddr, vector<ServiceRecord> serviceList);

	virtual CBlueToothSocket::CONNECT_STATUS_T GetDeviceConnStatus(BTH_ADDR deviceAddr);

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
