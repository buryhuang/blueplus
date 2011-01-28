#pragma once

#include <string>
using namespace std;

class CManagedObject
{
public:
	CManagedObject(void);
	CManagedObject(wstring name):m_name(name){}
	virtual ~CManagedObject(void);
	wstring GetName()
	{
		return m_name;
	}

private:
	wstring m_name;
};

class CRunnable
{
public:
	virtual int Run()=0;
};

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


class CManagedThread: public CManagedObject, public CRunnable
{
public:
	CManagedThread(void){}
	CManagedThread(wstring name):
		CManagedObject(name)
		,m_ptrThread(NULL)
	{
	}

	virtual ~CManagedThread(void)
	{
		if(m_ptrThread!=NULL){
			delete m_ptrThread;
		}
	};
	virtual BOOL IsAlive()
	{
		if(WaitForSingleObject(m_ptrThread->m_hThread, 0)!= WAIT_TIMEOUT)
		{
			return FALSE;
		}
		return TRUE;
	}
	virtual int Start()
	{
		m_ptrThread = new CPrivateManagedThread(this);
		m_ptrThread->CreateThread();
		return 0;
	}
	virtual int Restart()
	{
		if(m_ptrThread!=NULL){
			delete m_ptrThread;
		}
		m_ptrThread = new CPrivateManagedThread(this);
		m_ptrThread->CreateThread();
		return 0;
	}
	virtual void GraceShutdown()
	{
		WaitForSingleObject(m_ptrThread->m_hThread, INFINITE);
	}
protected:

	CPrivateManagedThread* m_ptrThread;
};
