#include "BTServer.h"
#include <iostream>
#include <fstream>
using namespace std;

class CBTServerHandler:public CSocketHandler, public CManagedThread
{
public:
	CBTServerHandler::CBTServerHandler(wstring name):CManagedThread(name){}
	virtual void OnAccept(SOCKET s)
	{
		m_pSocket = new CBlueToothSocket(s);
		wcout<<"Accepted "<<s<<endl;
		this->Start();
	}
	virtual void OnReceive(SOCKET,BYTEBUFFER)
	{
	};
	virtual void OnConnect(){};
	virtual void OnClose(){};
	virtual int Run()
	{
		fstream fs("gpsdemo.log");
		string buff;
		buff.resize(5000);
		while(fs.getline(&buff[0],5000)){
			buff.resize(fs.gcount());
			cout<<buff<<endl;
			wcout<<L"Byte sent: "<<m_pSocket->Send(buff)<<endl;
			buff.resize(5000);
			Sleep(2000);
		}
		return 0;
	}
private:
	CBlueToothSocket* m_pSocket;
};

CBTServer* CBTServer::m_instance=NULL;
CBTServer::CBTServer(void)
{
}

CBTServer::~CBTServer(void)
{
}

int CBTServer::Run()
{
	m_socket.Create(FALSE,FALSE);

	m_socket.Bind();

	while(true){
		m_socket.RegisterHandler(new CBTServerHandler(L"Default Connection Handler"));
		m_socket.Listen();
		m_socket.Accept();
		wcout<<L"Listen quitted"<<endl;

	}

	return 0;
}
