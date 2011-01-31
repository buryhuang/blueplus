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
	for(int cnt=0;cnt<m_rs.numFields();cnt++){
		wcout<<m_rs.fieldName(cnt)<<" - "<<m_rs.fieldDeclType(cnt)<<endl;
	}
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