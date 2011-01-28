// SmartDB.cpp : implementation file
//

#include "afxwin.h"
#include "SmartDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

HINSTANCE hDLL;
#if 0
typedef int				(* SMARTDB_OPEN)			(void *, sqlite3 **);
//typedef unsigned int			(* SMARTDB_EXEC)			(sqlite3*, const void *, sqlite3_callback, void *, char **);
typedef unsigned int			(* SMARTDB_CLOSE)			(sqlite3 *);
typedef int				(* SMARTDB_COLUMN_COUNT)	(sqlite3_stmt*);
typedef int				(* SMARTDB_PREPARE)			(sqlite3*, const void*, int, sqlite3_stmt**, const void**);
typedef int				(* SMARTDB_FINALIZE)		(sqlite3_stmt*);
typedef const void *	(* SMARTDB_COLUMN_NAME)		(sqlite3_stmt*, int iCol);
typedef int				(* SMARTDB_COLUMN_TYPE)		(sqlite3_stmt*, int iCol);
typedef int				(* SMARTDB_STEP)			(sqlite3_stmt*);
typedef int				(* SMARTDB_COLUMN_INT)		(sqlite3_stmt*, int iCol);
typedef const void *	(* SMARTDB_COLUMN_TEXT)		(sqlite3_stmt*, int iCol);
typedef const void *	(* SMARTDB_COLUMN_BLOB)		(sqlite3_stmt*, int iCol);
typedef double			(* SMARTDB_COLUMN_DOUBLE)	(sqlite3_stmt*, int iCol);

SMARTDB_OPEN			SmartDBOpen;
SMARTDB_EXEC			SmartDBExecute;
SMARTDB_CLOSE			SmartDBClose;
SMARTDB_COLUMN_COUNT	SmartDBColumnCount;    // Function pointer
SMARTDB_PREPARE			SmartDBPrepare;
SMARTDB_FINALIZE		SmartDBFinalize;
SMARTDB_COLUMN_NAME		SmartDBColumnName;
SMARTDB_COLUMN_TYPE		SmartDBColumnType;
SMARTDB_STEP			SmartDBStep;
SMARTDB_COLUMN_INT		SmartDBColumnInt;
SMARTDB_COLUMN_TEXT		SmartDBColumnText;
SMARTDB_COLUMN_BLOB		SmartDBColumnBlob;
SMARTDB_COLUMN_DOUBLE	SmartDBColumnDouble;
#endif 

#define	SmartDBOpen sqlite3_open16
#define	SmartDBExecute sqlite3_exec16 //added
#define	SmartDBClose sqlite3_close
#define	SmartDBColumnCount sqlite3_column_count
#define	SmartDBPrepare sqlite3_prepare16
#define	SmartDBFinalize sqlite3_finalize
#define	SmartDBColumnName sqlite3_column_name16
#define	SmartDBColumnType sqlite3_column_type
#define	SmartDBStep sqlite3_step
#define	SmartDBColumnInt sqlite3_column_int
#define	SmartDBColumnText sqlite3_column_text16
#define	SmartDBColumnBlob sqlite3_column_blob
#define	SmartDBColumnDouble sqlite3_column_double


/////////////////////////////////////////////////////////////////////////////
// SmartDB

// SmartDB constructor
CSmartDB::CSmartDB()
{
	InitInstance();
}

// Body of the distructor
CSmartDB::~CSmartDB()
{
#if 0
	if (m_bLibLoaded)
	{
		FreeLibrary(hDLL);
		m_bLibLoaded = FALSE;
	}
#endif
	m_bConnected = FALSE;
}

// This is called every time a new object is created
// and set new object's initial values
bool CSmartDB::InitInstance()
{
	m_bConnected = FALSE;
	m_bLibLoaded = TRUE;//FALSE
#if 0
	m_bLibLoaded = InitLibrary();
#endif
	return TRUE;
}

// Try to load the exported functions of Sqlite DLL
// Called from InitInstance
bool CSmartDB::InitLibrary(void)
{
	bool bRetVal = FALSE;
#if 0
	hDLL = LoadLibrary(L"sqlite3");
	if (hDLL)
	{
		SmartDBOpen = (SMARTDB_OPEN)GetProcAddress(hDLL, "sqlite3_open16");
		//SmartDBExecute = (SMARTDB_EXEC)GetProcAddress(hDLL, "sqlite3_exec");
		SmartDBClose = (SMARTDB_CLOSE)GetProcAddress(hDLL, "sqlite3_close");
		SmartDBColumnCount = (SMARTDB_COLUMN_COUNT)GetProcAddress(hDLL, "sqlite3_column_count");
		SmartDBPrepare = (SMARTDB_PREPARE)GetProcAddress(hDLL, "sqlite3_prepare16");
		SmartDBFinalize = (SMARTDB_FINALIZE)GetProcAddress(hDLL, "sqlite3_finalize");
		SmartDBColumnName = (SMARTDB_COLUMN_NAME)GetProcAddress(hDLL, "sqlite3_column_name16");
		SmartDBColumnType = (SMARTDB_COLUMN_TYPE)GetProcAddress(hDLL, "sqlite3_column_type");
		SmartDBStep = (SMARTDB_STEP)GetProcAddress(hDLL, "sqlite3_step");
		SmartDBColumnInt = (SMARTDB_COLUMN_INT)GetProcAddress(hDLL, "sqlite3_column_int");
		SmartDBColumnText = (SMARTDB_COLUMN_TEXT)GetProcAddress(hDLL, "sqlite3_column_text16");
		SmartDBColumnBlob = (SMARTDB_COLUMN_BLOB)GetProcAddress(hDLL, "sqlite3_column_blob");
		SmartDBColumnDouble = (SMARTDB_COLUMN_DOUBLE)GetProcAddress(hDLL, "sqlite3_column_double");

		if (!SmartDBOpen /*|| !SmartDBExecute*/
				|| !SmartDBClose		|| !SmartDBColumnCount
				|| !SmartDBPrepare		|| !SmartDBFinalize
				|| !SmartDBColumnName	|| !SmartDBColumnType
				|| !SmartDBStep			|| !SmartDBColumnInt
				|| !SmartDBColumnText	|| !SmartDBColumnBlob
				|| !SmartDBColumnDouble )
			m_bLibLoaded = FALSE;
		else
			m_bLibLoaded = TRUE;
	}
	if (m_bLibLoaded == FALSE)
		FreeLibrary (hDLL);
#endif
	return m_bLibLoaded;
}

/////////////////////////////////////////////////////////////////////////////
// SmartDB message handlers

// Try to Open the given database
// Returns TRUE on success
bool CSmartDB::Connect(wstring strDBPathName, wstring strParamString)
{
	if (m_bLibLoaded == FALSE)
		return FALSE;

	if (SmartDBOpen(&strDBPathName[0], &db))	// If a non zero is returned, some problem occured
	{
		SmartDBClose(db);
		return FALSE;
	}

	m_bConnected = TRUE;
	return TRUE;
}

// Try to close the currently associated connection
bool CSmartDB::Close()
{
	SmartDBClose(db);
	m_bConnected = FALSE;
	return TRUE;
}

// Check if a connection is established or not
bool CSmartDB::IsConnected()
{
	return m_bConnected;
}

// Try to execute an SQL query
// Returns ZERO on success
unsigned int CSmartDB::Execute(wstring strQuery, unsigned int *nRecEfected)
{
	unsigned int nRetValue;
	char *zErrMsg = 0;
	if (SmartDBExecute (db, &strQuery[0], NULL, 0, &zErrMsg) == SQLITE_OK)
		nRetValue = 0;
	else
	{
		nRetValue = 1;
	}
	return nRetValue;
}

// Return TRUE if Dynamic Link Library is loaded
// correctlly by Object Creation
bool CSmartDB::IsLibLoaded ()
{
	return m_bLibLoaded;
}

// SmartDBRecordSet.cpp : implementation file
//

/////////////////////////////////////////////////////////////////////////////
// CSmartDBRecordSet

// CSmartDBRecordSet constructor
CSmartDBRecordSet::CSmartDBRecordSet()
{
	InitInstance();
}

// Body of the distructor
CSmartDBRecordSet::~CSmartDBRecordSet()
{
	if (m_bIsOpen)
		Close();
}

// This is called every time a new object is created
// and set new object's initial values
bool CSmartDBRecordSet::InitInstance()
{
	m_bIsOpen = FALSE;
	m_nCurrentRecord = -1;
	m_nRecordCount = -1;
	m_nPageSize = 50;
	m_nPageCount = 0;
	m_nCurrentPage = 0;
	m_bIsProcessing = FALSE;
	m_nFieldsCount = 0;
	m_bIsOpen = FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSmartDBRecordSet message handlers

// Execute SQL Query for the established connection
// Mostly a SELECT query, Other queries may be executed using Execute()
// Returns TRUE on success
unsigned int CSmartDBRecordSet::Open(wstring strQueryString, CSmartDB *ptrConn, LONG nRSType)
{
	if (ptrConn->IsLibLoaded() == FALSE)
		return RSOPEN_NOLIBLOADED;

	if (m_bIsOpen)
		return RSOPEN_ALREADYOPENED;

	if (!ptrConn->IsConnected())
		return RSOPEN_NOCONNECT;
	
	wstring strBuffer;			// Temp Buffer
	vector<wstring> * strRSRow;		// Record Set holder
	bool bTypesSaved = FALSE;	// Flag used to mark if DataTypes are saved for each column
	char *zErrMsg = 0;

	sqlite3_stmt *stmtByteCode;
	const void *strUnused = 0;
	unsigned int nCount = 0;
	unsigned int nRecCount = 0;
	int nType;

	strBuffer = strQueryString;
	SmartDBPrepare(ptrConn->db, &strBuffer[0], int(strBuffer.size()), &stmtByteCode, &strUnused);

	nCount = SmartDBColumnCount(stmtByteCode);
	for (unsigned int i=0; i < nCount; i++)
	{
		strBuffer = (wchar_t *)SmartDBColumnName (stmtByteCode, i); // column_name
		strFieldsList.push_back (strBuffer);
	}
	m_nFieldsCount = int(strFieldsList.size());
	

	// return, If there is no field in the table
	if (m_nFieldsCount == 0)
		return RSOPEN_INVALIDQRY;

	m_bIsOpen = TRUE;

	int nValue;
	double nFValue;
	wstring strValue;
	wstring strTemp;
	wistringstream iss;

	while (SmartDBStep (stmtByteCode) != SQLITE_DONE)	// step
	{
		strRSRow = new vector<wstring>();

		for (int i=0; (LONG)i < m_nFieldsCount; i++)
		{
			if (!bTypesSaved)	// Save Field Types in an array
			{
				nType = SmartDBColumnType (stmtByteCode, i);	// column_type
				nFieldsType.push_back((unsigned int)nType);
				if ((LONG)i == m_nFieldsCount-1)
					bTypesSaved = TRUE;
			}
			
			switch (nFieldsType[i])
			{
				case SQLITE_INTEGER:
					nValue = SmartDBColumnInt (stmtByteCode, i);
					iss.clear();iss>>nValue;
					strTemp=iss.str();
					strRSRow->push_back(strTemp);
					break;
				case SQLITE_FLOAT:
					nFValue = SmartDBColumnDouble (stmtByteCode, i);
					iss.clear();iss>>nFValue;
					strTemp=iss.str();
					strRSRow->push_back(strTemp);
					break;
				case SQLITE_TEXT:
					strValue = (wchar_t *)SmartDBColumnText (stmtByteCode, i);
					strRSRow->push_back(strValue);
					break;
				case SQLITE_BLOB:
					strValue = (wchar_t *)SmartDBColumnBlob (stmtByteCode, i);
					strRSRow->push_back(strValue);
					break;
				case SQLITE_NULL:
					strRSRow->push_back(L"");
					break;
			}
		}
		orsRows.push_back((void *)strRSRow);
		nRecCount++;
	}

	m_nRecordCount = nRecCount;
	m_nCurrentRecord = 0;

	SmartDBFinalize(stmtByteCode);

	return RSOPEN_SUCCESS;
}

// Try to close the currently opened recordset
// Returns TRUE on success
bool CSmartDBRecordSet::Close()
{
	if (!m_bIsOpen)
		return FALSE;

	vector<wstring> *strRSRow;

	if (m_bIsOpen)
	{
		for (LONG i = m_nRecordCount-1; i >= 0; i--)
		{
			strRSRow = (vector<wstring> *)orsRows[i];
			delete (strRSRow);
		}
		orsRows.clear();

		// Clear field list
		strFieldsList.clear();
		nFieldsType.clear();

		m_nFieldsCount = int(strFieldsList.size());
		m_bIsOpen = FALSE;
		return TRUE;
	}
	else
		return FALSE;
}

// Returns number of rows in the current RecordSet
LONG CSmartDBRecordSet::RecordCount()
{
	if (m_bIsOpen)
		return m_nRecordCount;
	else
		return (-1L);
}

// Returns TRUE if record pointer has skipped the last record
bool CSmartDBRecordSet::IsEOF()
{
	if (!m_bIsOpen)
		return TRUE;

	return (m_nCurrentRecord == m_nRecordCount);
}

// Move the Record pointer to the very first row of the record set
void CSmartDBRecordSet::MoveFirst()
{
	ASSERT (m_bIsOpen);

	m_nCurrentRecord = 0;
	return;
}

// Move the Record pointer to the last row of the record set
void CSmartDBRecordSet::MoveLast()
{
	ASSERT (m_bIsOpen);

	m_nCurrentRecord = m_nRecordCount - 1;
	return;
}

// Move the Record pointer to the previous row
void CSmartDBRecordSet::MovePrevious()
{
	ASSERT (m_bIsOpen);

	if (m_nCurrentRecord > 0)
		m_nCurrentRecord--;
	return;
}

// Move the Record pointer to the previous row
void CSmartDBRecordSet::MoveNext()
{
	ASSERT (m_bIsOpen);

	if (m_nCurrentRecord < m_nRecordCount)
		m_nCurrentRecord++;
	return;
}

// Returns the Name of the Field (or Column) whose
// index is given
wstring CSmartDBRecordSet::GetFieldName(LONG nIndex)
{
	ASSERT (m_bIsOpen);

	if (nIndex < static_cast<int>(strFieldsList.size()))
		return strFieldsList[nIndex];
	else
		return NULL;
}

// Returns the Type of the Field (or Column) whose
// index is given
int CSmartDBRecordSet::GetFieldType(LONG nIndex)
{
	if (nIndex < static_cast<int>(nFieldsType.size()))
		return (int)nFieldsType[nIndex];
	else
		return 0;
}

// Returns the Total number of Columns in the record set
unsigned int CSmartDBRecordSet::FieldsCount()
{
	ASSERT (m_bIsOpen);

	return m_nFieldsCount;
}


// Returns the value of the column index in string format
// for current row in the record set
wstring CSmartDBRecordSet::GetColumnString(LONG nIndex)
{
	if (m_nCurrentRecord < 0 || m_nCurrentRecord >= m_nRecordCount || nIndex >= m_nFieldsCount)
		return NULL;

	vector<wstring> * strArr;
	strArr = (vector<wstring> *)orsRows[m_nCurrentRecord];
	
	return (*strArr)[nIndex];
}
