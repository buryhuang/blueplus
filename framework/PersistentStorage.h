#pragma once
#include "managedobject.h"

#include "CppSQLite3U.h"

#include <string>
using namespace std;

#define PERSISTENT_STORAGE_PTR (CPersistentStorage::GetInstance(L"Default"))

class CPersistentStorageImpl;

class CPersistentStorage :
	public CManagedObject
{
public:
	CPersistentStorage(void){}
	CPersistentStorage(wstring name):CManagedObject(name){}
	static CPersistentStorage* GetInstance(wstring name);
	virtual ~CPersistentStorage(void){}

	virtual void InsertTimedValues(wstring table, wstring values)=0;
private:
	static CPersistentStorage* m_instance;
};


class CPersistentStorageImpl :
	public CPersistentStorage
{
public:
	CPersistentStorageImpl(void);
	CPersistentStorageImpl(wstring name);

	virtual ~CPersistentStorageImpl(void);
	virtual void InsertTimedValues(wstring table, wstring values);

protected:
	CppSQLite3DB m_db;
	CppSQLite3Query m_rs;
};


