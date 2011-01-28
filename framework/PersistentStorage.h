#pragma once
#include "managedobject.h"
#include <string>
using namespace std;

class CPersistentStorage :
	public CManagedObject
{
public:
	CPersistentStorage(void){}
	CPersistentStorage(wstring name):CManagedObject(name){}
	virtual ~CPersistentStorage(void){}

	virtual void InsertString(wstring string)=0;
};

class CPersistentStorageImpl :
	public CPersistentStorage
{
public:
	CPersistentStorageImpl(void);
	CPersistentStorageImpl(wstring name):CPersistentStorage(name){}

	virtual ~CPersistentStorageImpl(void);
	virtual void InsertString(wstring string);

private:

};


