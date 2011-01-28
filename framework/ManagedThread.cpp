#include "ManagedThread.h"
#include "afxwin.h"

class CPrivateManagedThread: public CWinThread
{
public:
	CPrivateManagedThread(CRunnable* ptr):m_ptrRunnable(ptr)
	{
	}
	BOOL InitInstance()
	{
		m_bAutoDelete=FALSE;
		return TRUE;
	}
	virtual int Run()
	{
		return m_ptrRunnable->Run();
	}

private:
	CRunnable* m_ptrRunnable;
};


CManagedThread::CManagedThread(void){}
CManagedThread::CManagedThread(wstring name):
	CManagedObject(name)
	,m_ptrThread(NULL)
{
}

CManagedThread::~CManagedThread(void)
{
	if(m_ptrThread!=NULL){
		delete m_ptrThread;
	}
};
bool CManagedThread::IsAlive()
{
	if(WaitForSingleObject(m_ptrThread->m_hThread, 0)!= WAIT_TIMEOUT)
	{
		return FALSE;
	}
	return TRUE;
}
int CManagedThread::Start()
{
	m_ptrThread = new CPrivateManagedThread(this);
	m_ptrThread->CreateThread();
	return 0;
}
int CManagedThread::Restart()
{
	if(m_ptrThread!=NULL){
		delete m_ptrThread;
	}
	m_ptrThread = new CPrivateManagedThread(this);
	m_ptrThread->CreateThread();
	return 0;
}
void CManagedThread::GraceShutdown()
{
	WaitForSingleObject(m_ptrThread->m_hThread, INFINITE);
}
