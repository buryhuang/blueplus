/** \addtogroup framework
 *  @{
 */

#include "PersistentStorage.h"
#include <iostream>
#include <string>
#include <sstream>
#include <..\Utils.h>
using namespace std;

const LPCTSTR DBFILE = L"bppmaster.db";

CPersistentStorage* CPersistentStorage::m_instance=NULL;
CPersistentStorage* CPersistentStorage::GetInstance(wstring name)
{
	if(m_instance == NULL){
		m_instance = new CPersistentStorageImpl(name);
	}
	return m_instance;
}


CPersistentStorageImpl::CPersistentStorageImpl(wstring name):CPersistentStorage(name)
{
	m_db.open(DBFILE);
	m_rs = m_db.execQuery(L"SELECT * FROM events");
}

CPersistentStorageImpl::~CPersistentStorageImpl(void)
{
	m_db.close();
}

void CPersistentStorageImpl::InsertTimedValues(wstring table, wstring values)
{
	wstringstream ss;
	wstring query;
	ss<<L"INSERT INTO ";
	ss<<table<<L" values(";
	ss<<L"'"<<Utils::CurrentTime()<<L"'";
	ss<<L","<<values;
	ss<<L")";
	query = ss.str();
	m_rs = m_db.execQuery(query.c_str());
}

CEvent CPersistentStorageImpl::GetLastRecord()
{
	CEvent result;
	CppSQLite3Query rs;

	rs = m_db.execQuery(L"select * from events order by rowid desc limit 1");

	if(rs.eof()==FALSE){
		result.timestamp=wstring(rs.getStringField(L"timestamp"));
		result.source_id=rs.getStringField(L"source_id");
		result.source_desc=rs.getStringField(L"source_desc");
		result.event_id=rs.getIntField(L"event_id");
		result.event_desc=rs.getStringField(L"event_desc");
		result.isNull=FALSE;
		return result;
	}
	result.isNull=TRUE;
	return result;

}

bool CPersistentStorageImpl::NextRecord()
{
	m_rs.nextRow();
	if(m_rs.eof()==FALSE){
		return TRUE;
	}
	return FALSE;
}

CEvent CPersistentStorageImpl::GetRecord()
{
	CEvent result;
	if(m_rs.eof()==FALSE){
		result.timestamp=wstring(m_rs.getStringField(L"timestamp"));
		result.source_id=m_rs.getStringField(L"source_id");
		result.source_desc=m_rs.getStringField(L"source_desc");
		result.event_id=m_rs.getIntField(L"event_id");
		result.event_desc=m_rs.getStringField(L"event_desc");
		result.isNull=FALSE;
		return result;
	}
	result.isNull=TRUE;
	return result;
}

#ifdef UNITTEST
#include "..\\unittest_config.h"
#include "gtest/gtest.h"

TEST(CPersistentStorageTest,Reading)
{
	while(PERSISTENT_STORAGE_PTR->NextRecord()){
		CEvent evt = PERSISTENT_STORAGE_PTR->GetRecord();
		ASSERT_TRUE(evt.isNull != TRUE);
#if 0

		if(evt.isNull==FALSE){
			wcout<<evt.timestamp<<"\t";
			wcout<<evt.source_id<<"\t";
			wcout<<evt.source_desc<<"\t";
			wcout<<evt.event_id<<"\t";
			wcout<<evt.event_desc<<"\t";
			wcout<<endl;
		}

#endif

	}

}

TEST(CPersistentStorageTest,Writing)
{
	PERSISTENT_STORAGE_PTR->InsertTimedValues(L"events",L"'UTEST_SRCID','UTEST_SRCDESC',999999,'UTEST_EVTDESC'");
	CEvent evt = PERSISTENT_STORAGE_PTR->GetLastRecord();
	if(evt.isNull==FALSE){
		wcout<<evt.timestamp<<"\t";
		wcout<<evt.source_id<<"\t";
		wcout<<evt.source_desc<<"\t";
		wcout<<evt.event_id<<"\t";
		wcout<<evt.event_desc<<"\t";
		wcout<<endl;
		ASSERT_TRUE(evt.source_id==L"UTEST_SRCID");
		ASSERT_TRUE(evt.source_desc==L"UTEST_SRCDESC");
		ASSERT_TRUE(evt.event_id==999999);
		ASSERT_TRUE(evt.event_desc==L"UTEST_EVTDESC");
	}
}

#endif


/** @}*/
