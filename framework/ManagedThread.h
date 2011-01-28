#include "ManagedObject.h"

class CPrivateManagedThread;

class CRunnable
{
public:
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

protected:

	CPrivateManagedThread* m_ptrThread;
};
