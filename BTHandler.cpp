#include "StdAfx.h"
#include "BTHandler.h"

#include <iostream>

BTHandler::BTHandler(void)
{
}

BTHandler::~BTHandler(void)
{
}

void BTHandler::OnDeviceDiscovered(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, BOOL paired)
{
	wcout<<deviceAddr<<" - "<<deviceName<<endl;
}
