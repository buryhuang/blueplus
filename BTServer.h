#pragma once

#include "managedthread.h"
#include "BluetoothSocket.h"

#define DEF_BTSERVER CBTServer::GetInstance(L"BT Server")

class CBTServer :
	public CManagedThread
{
public:
	CBTServer(void);
	CBTServer(wstring name):CManagedThread(name){};
	virtual ~CBTServer(void);

	static CBTServer* GetInstance(wstring name){
		if(m_instance==NULL){
			m_instance = new CBTServer(name);
		}
		return m_instance;
	}
	virtual int Run();

private:
	static CBTServer* m_instance;
	CBlueToothSocket m_socket;
};
