#include "BTServer.h"
#include <iostream>
using namespace std;

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
		m_socket.Listen();
		m_socket.Accept();
		wcout<<L"Listen quitted"<<endl;
	}

	return 0;
}
