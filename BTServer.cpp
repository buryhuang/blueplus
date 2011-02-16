/** \addtogroup application
 *  @{
 */

#include "BTServer.h"
#include <iostream>
#include <fstream>
using namespace std;

#ifdef UNITTEST
static BYTEBUFFER m_lastBuff;
#endif

class CBTServerHandler:public CSocketHandler, public CManagedThread
{
public:

	CBTServerHandler::CBTServerHandler(wstring name):CManagedThread(name){}
	virtual void OnAccept(SOCKET s)
	{
		m_pSocket = new CBlueToothSocket(s);
		m_pSocket->RegisterHandler(this);
		wcout<<"Accepted "<<s<<endl;
		this->Start();
	}
	virtual void OnReceive(SOCKET s, BYTEBUFFER buff)
	{
		cout<<"Received: "<<buff<<endl;
		#ifdef UNITTEST
		m_lastBuff=buff;
		#endif
	};
	virtual void OnConnect(){};
	virtual void OnClose(){};
	virtual int Run()
	{
#if 0
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
#endif
		m_pSocket->Recveive();

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

		if(m_socket.RegisterHandler(new CBTServerHandler(L"Default Connection Handler"))!=TRUE){
			Utils::ShowError(L"CBTServer::Run");
			return -1;
		}
		if(m_socket.Listen()!=TRUE){
			Utils::ShowError(L"CBTServer::Run");
			return -1;
		}
		if(m_socket.Accept()==0){
			Utils::ShowError(L"CBTServer::Run");
			return -1;
		}
		wcout<<L"Connection established..."<<endl;
	}

	return 0;
}

#ifdef UNITTEST
#include "unittest_config.h"
#include "gtest/gtest.h"

TEST(CBTServerTest,Init)
{
	ASSERT_TRUE(DEF_BTSERVER != NULL);
}

TEST(CBTServerTest,CreateServer)
{
	ASSERT_EQ(DEF_BTSERVER->Start(),0);
}

TEST(CBTServerTest,ConnectToLocalServer)
{
	CBlueToothSocket cbts;

	CBlueTooth cbt;
	SOCKADDR_BTH btAddr;
	cbt.GetLocalAddress(btAddr);
	cbts.Create(FALSE,FALSE);
	cbts.Connect(btAddr.btAddr,1,5);

	ASSERT_TRUE(WSAGetLastError()==0);

	string testStr = "BTSERVER TEST SENDING";
	cbts.Send(testStr);
	Sleep(1000);//Arbitrary, just to wait for server to receive
	ASSERT_TRUE(testStr ==m_lastBuff);
	DEF_BTSERVER->Shutdown();
}

TEST(CBTServerTest,GraceShutdown)
{
	DEF_BTSERVER->GraceShutdown();
}
	
#endif

/** @}*/
