/** \addtogroup framework
 *  @{
 */

#include "ManagedThread.h"
#include "windows.h"
//#include "afxwin.h"

class CPrivateManagedThread
{
public:
	CPrivateManagedThread(CRunnable* ptr):m_ptrRunnable(ptr)
	{
	}
	
	bool InitInstance()
	{
		m_bAutoDelete=false;
		return true;
	}

	static UINT ThreadFunc(LPVOID param)
   {
      CPrivateManagedThread* This = (CPrivateManagedThread*)param;
      This->Run(); // call a member function
	  return 0;
   }

	void CreateThread()
	{
		m_hThread = ::CreateThread (
            0, // Security attributes
            0, // Stack size
			(LPTHREAD_START_ROUTINE)(ThreadFunc),
            (LPVOID)this,
            0/*CREATE_SUSPENDED*/,
            &m_tid);
	}

	virtual int Run()
	{
		return m_ptrRunnable->Run();
	}

	HANDLE     m_hThread;

private:
	CRunnable* m_ptrRunnable;
	DWORD      m_tid;
	bool       m_bAutoDelete;
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
		return false;
	}
	return true;
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

void CManagedThread::Shutdown()
{
	TerminateThread(m_ptrThread->m_hThread,0);
}


/** @}*/
