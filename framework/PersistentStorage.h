/** \addtogroup framework
 *  @{
 */

#pragma once
#include "managedobject.h"

#include "CppSQLite3U.h"

#include <string>
#include <map>
using namespace std;

#define PERSISTENT_STORAGE_PTR (CPersistentStorage::GetInstance(L"Default"))

struct CEvent
{
public:
	wstring timestamp;
	wstring source_id;
	wstring source_desc;
	long long event_id;
	wstring event_desc;
	bool isNull;
};

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
	virtual bool NextRecord()=0;
	virtual CEvent GetRecord()=0;
	virtual CEvent GetLastRecord()=0;
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
	virtual bool NextRecord();
	virtual CEvent GetRecord();
	virtual CEvent GetLastRecord();

protected:
	CppSQLite3DB m_db;
	CppSQLite3Query m_rs;
};



/** @}*/
