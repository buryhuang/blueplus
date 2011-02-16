/** \addtogroup framework
 *  @{
 */

#pragma once

#include "ManagedObject.h"

class CPrivateManagedThread;

class CRunnable
{
public:
	//DO NOT call directly, this is called by CPrivateManagedThread
	virtual int Run()=0;
};


class CManagedThread: public CManagedObject, public CRunnable
{
public:
	CManagedThread(void);
	CManagedThread(wstring name);

	virtual ~CManagedThread(void);
	virtual bool IsAlive();
	virtual int Start();
	virtual int Restart();
	virtual void GraceShutdown();
	virtual void Shutdown(); //Do NOT use this until very neccessary

protected:

	CPrivateManagedThread* m_ptrThread;
};


/** @}*/
