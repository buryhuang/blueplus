#if !defined(__SMARTDB_)
#define __SMARTDB_

#if _MSC_VER > 1000
#pragma once
#endif

// SmartDB.h : header file
//
#include <string>
#include <vector>
using namespace std;

#define RSOPEN_SUCCESS			0
#define RSOPEN_NOLIBLOADED		1
#define RSOPEN_NOCONNECT		2
#define RSOPEN_ALREADYOPENED	3
#define RSOPEN_INVALIDQRY		4


#include "sqlite3.h"


/////////////////////////////////////////////////////////////////////////////
// CSmartDB thread

class CSmartDB
{
	// Attributes
	public:
		CSmartDB();           // constructor and distructor
		~CSmartDB();

	// Operations
	public:
		bool Connect (wstring strDBPathName, wstring strParamString = NULL);
		bool InitInstance();
		bool Close (void);
		bool IsConnected (void);
		unsigned int Execute (wstring strQuery, unsigned int * nRecEfected = NULL);
		bool IsLibLoaded (void);

		sqlite3 *db;

	// Implementation
	private:
		bool InitLibrary(void);

		bool m_bLibLoaded;
		bool m_bConnected;
};


/////////////////////////////////////////////////////////////////////////////
// CSmartDBRecordSet thread

class CSmartDBRecordSet
{
	// Attributes
	public:
		CSmartDBRecordSet();           // protected constructor used by dynamic creation
		virtual ~CSmartDBRecordSet();

	// Operations
	public:
		bool Close (void);
		unsigned int Open (wstring strQueryString, CSmartDB * ptrConn, long nRSType = NULL);
		bool InitInstance();

		unsigned int FieldsCount (void);
		wstring GetFieldName (long nIndex);
		int GetFieldType (long nIndex);
		
		long RecordCount (void);
		void MoveNext (void);
		void MovePrevious (void);
		void MoveLast (void);
		void MoveFirst (void);
		bool IsEOF (void);

		wstring GetColumnString (long nIndex);

	// Implementation
	private:
		bool m_bIsOpen;
		vector<wstring> strFieldsList;
		vector<unsigned int> nFieldsType;
		vector<void *> orsRows;
		long m_nFieldsCount;
		bool m_bIsProcessing;
		long m_nCurrentPage;
		long m_nPageCount;
		long m_nPageSize;
		long m_nCurrentRecord;
		long m_nRecordCount;
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__SMARTDB_)
