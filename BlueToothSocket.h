#pragma once

#include "BlueTooth.h"
#include "Utils.h"

class CBlueToothSocket
{
public:
	BOOL   Create(BOOL authenticate, BOOL encrypt);
	BOOL   Connect(long socket, long address, int channel, int retryUnreachable);
	BOOL   Bind(long socket);
	BOOL   Listen(long socket);
	SOCKET Accept(long socket);
	int    RecveiveAvailable(long socket);
	void   Close(long socket);
	int    Recveive(long socket);
	size_t Recveive(long socket, BYTEBUFFER buff);
	size_t Send(long socket, BYTEBUFFER buff);

protected:
	SOCKET m_socket;
};