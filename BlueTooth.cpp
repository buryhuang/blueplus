#include "BlueTooth.h"

#include "Utils.h"

#include <iostream>
using namespace std;

CBlueTooth::CBlueTooth(void):
	m_bBluetoothStackPresent(FALSE),
	m_bStarted(FALSE),
	m_hDeviceLookup(NULL),
	m_bInitialBtIsDiscoverable(FALSE),
	m_bRestoreBtMode(FALSE)
{
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		WSAGetLastError();
		m_bStarted = FALSE;
	} else {
		m_bStarted = TRUE;
    }
}

CBlueTooth::~CBlueTooth(void)
{
	if (m_bStarted) {
		if (m_bRestoreBtMode) {
			BluetoothEnableDiscovery(NULL, m_bInitialBtIsDiscoverable);
            m_bRestoreBtMode = false;
		}
		WSACleanup();
	}
}

bool CBlueTooth::GetLocalAddress(SOCKADDR_BTH& btAddr)
{
	if(InitializationStatus()==FALSE){
		return FALSE;
	}
	SOCKET s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	//Utils::ShowError(TEXT("isBluetoothStackPresent"));
	if (s == INVALID_SOCKET) {
		int last_error = WSAGetLastError();
		//debug(("socket error [%d] %S", last_error, getWinErrorMessage(last_error)));
		Utils::ShowError(TEXT("isBluetoothStackPresent"));
		return FALSE;
	}

	memset(&btAddr, 0, sizeof(SOCKADDR_BTH));
	btAddr.addressFamily = AF_BTH;

	btAddr.port = BT_PORT_ANY;

	if (bind(s, (SOCKADDR *)&btAddr, sizeof(SOCKADDR_BTH))) {
		int last_error = WSAGetLastError();
		//debug(("bind error [%d] %S", last_error, getWinErrorMessage(last_error)));
		Utils::ShowError(TEXT("isBluetoothStackPresent"));
		closesocket(s);
		return FALSE;
	}

	int size = sizeof(SOCKADDR_BTH);
	if (getsockname(s, (sockaddr*)&btAddr, &size)) {
		int last_error = WSAGetLastError();
//		debug(("getsockname error [%d] %S", last_error, getWinErrorMessage(last_error)));
		closesocket(s);
		return FALSE;
	}
	closesocket(s);
	return TRUE;
}

bool CBlueTooth::IsBluetoothStackPresent() {

	SOCKET s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	//Utils::ShowError(TEXT("isBluetoothStackPresent"));
	if (s == INVALID_SOCKET) {
		int last_error = WSAGetLastError();
		//debug(("socket error [%d] %S", last_error, getWinErrorMessage(last_error)));
		Utils::ShowError(TEXT("isBluetoothStackPresent"));
		return FALSE;
	}
	SOCKADDR_BTH btAddr;
	memset(&btAddr, 0, sizeof(SOCKADDR_BTH));
	btAddr.addressFamily = AF_BTH;
#ifdef _WIN32_WCE
	btAddr.port = 0;
#else
	btAddr.port = BT_PORT_ANY;
#endif
	if (bind(s, (SOCKADDR *)&btAddr, sizeof(SOCKADDR_BTH))) {
		int last_error = WSAGetLastError();
		//debug(("bind error [%d] %S", last_error, getWinErrorMessage(last_error)));
		Utils::ShowError(TEXT("isBluetoothStackPresent"));
		closesocket(s);
		return FALSE;
	}

	int size = sizeof(SOCKADDR_BTH);
	if (getsockname(s, (sockaddr*)&btAddr, &size)) {
		int last_error = WSAGetLastError();
//		debug(("getsockname error [%d] %S", last_error, getWinErrorMessage(last_error)));
		closesocket(s);
		return FALSE;
	}

	closesocket(s);
	//return TRUE;
	m_bBluetoothStackPresent = (btAddr.btAddr != 0);
	return m_bBluetoothStackPresent;
}

bool CBlueTooth::InitializationStatus() {
    if (!m_bBluetoothStackPresent) {
        if (!IsBluetoothStackPresent()) {
            //throwBluetoothStateException(env, "BluetoothStack not detected");
			Utils::ShowError(TEXT("initializationStatus"));
        }
    }

	if (m_bStarted) {
		if (BluetoothIsDiscoverable(NULL)) {
			m_bInitialBtIsDiscoverable = TRUE;
		}
		return TRUE;
    }
    return FALSE;
}

bool CBlueTooth::GetBluetoothGetRadioInfo(long address, BLUETOOTH_RADIO_INFO* info) {
	HANDLE hRadio;
	BLUETOOTH_FIND_RADIO_PARAMS btfrp = { sizeof(btfrp) };
	HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio( &btfrp, &hRadio );
	if ( NULL != hFind ) {
		do {
			BLUETOOTH_RADIO_INFO radioInfo;
			radioInfo.dwSize = sizeof(radioInfo);
			if (ERROR_SUCCESS == BluetoothGetRadioInfo(hRadio, &radioInfo)) {
				if (radioInfo.address.ullLong == address) {
					BluetoothFindRadioClose(hFind);
					memcpy(info, &radioInfo, sizeof(BLUETOOTH_RADIO_INFO));
					return TRUE;
				}
			}
		} while( BluetoothFindNextRadio( hFind, &hRadio ) );
		BluetoothFindRadioClose( hFind );
	}
	return FALSE;
}

string CBlueTooth::GetRadioName(long address) {
	BLUETOOTH_RADIO_INFO radioInfo;
	if (GetBluetoothGetRadioInfo(address, &radioInfo)) {
		return string((char*)radioInfo.szName, (int) wcslen(radioInfo.szName));
	}
	return NULL;
}

int CBlueTooth::GetDeviceVersion(long address)
{
	BLUETOOTH_RADIO_INFO radioInfo;
	if (GetBluetoothGetRadioInfo(address, &radioInfo)) {
		return radioInfo.lmpSubversion;
	}
	return -1;
}

int CBlueTooth::GetDeviceManufacturer(long address) {
	BLUETOOTH_RADIO_INFO radioInfo;
	if (GetBluetoothGetRadioInfo(address, &radioInfo)) {
		return radioInfo.manufacturer;
	}
	return -1;
}

int CBlueTooth::RunDeviceInquiry(int duration)
{
	// build device query

	BTH_QUERY_DEVICE query;
	query.LAP = 0;//accessCode; MSDN: Reserved. Must be set to zero.

	query.length = (unsigned char)duration;

	// build BLOB pointing to device query

	BLOB blob;

	blob.cbSize = sizeof(query);
	blob.pBlobData = (BYTE *)&query;

	// build query

	WSAQUERYSET queryset;

	memset(&queryset, 0, sizeof(WSAQUERYSET));
	queryset.dwSize = sizeof(WSAQUERYSET);
	queryset.dwNameSpace = NS_BTH;

	// TODO Test this.
	//queryset.lpBlob = &blob;

	queryset.lpBlob = &blob;

	// begin query

	if (m_hDeviceLookup != NULL) {
		//throwBluetoothStateException(env, cINQUIRY_RUNNING);
		return INQUIRY_ERROR;
	}

	if (WSALookupServiceBegin(&queryset, LUP_FLUSHCACHE|LUP_CONTAINERS, &m_hDeviceLookup)) {
		//int last_error = WSAGetLastError();

		//throwBluetoothStateExceptionWinErrorMessage(env, "Can't start Lookup", last_error);
//		debug(("WSALookupServiceBegin error [%d] %S", last_error, getWinErrorMessage(last_error)));
		Utils::ShowError(TEXT("runDeviceInquiry"));
		return INQUIRY_ERROR;
	}

	// fetch results
    int result = -1;

	int bufSize = 0x2000;
	void* buf = malloc(bufSize);
	if (buf == NULL) {
		result = INQUIRY_ERROR;
	}

	while (result == -1) {
		memset(buf, 0, bufSize);

		LPWSAQUERYSET pwsaResults = (LPWSAQUERYSET) buf;
		pwsaResults->dwSize = sizeof(WSAQUERYSET);
		pwsaResults->dwNameSpace = NS_BTH;

		DWORD size = bufSize;

		if (m_hDeviceLookup == NULL) {
			result = INQUIRY_TERMINATED;
			//debug(("doInquiry, INQUIRY_TERMINATED"));
			break;
		}
        //debug(("doInquiry, WSALookupServiceNext"));
		if (WSALookupServiceNext(m_hDeviceLookup, LUP_RETURN_NAME|LUP_RETURN_ADDR|LUP_RETURN_BLOB, &size, pwsaResults)) {
			int last_error = WSAGetLastError();
			switch(last_error) {
				case WSAENOMORE:
			    case WSA_E_NO_MORE:
				    result = INQUIRY_COMPLETED;
					break;
			    default:
//					debug(("Device lookup error [%d] %S", last_error, getWinErrorMessage(last_error)));
					Utils::ShowError(TEXT("runDeviceInquiry"));
				    result = INQUIRY_ERROR;
			}
			WSALookupServiceEnd(m_hDeviceLookup);
			m_hDeviceLookup = NULL;
			
			//debug(("doInquiry, exits"));
		break;
		}


        //debug(("doInquiry, has next Service"));

		BTH_DEVICE_INFO *p_inqRes = (BTH_DEVICE_INFO *)pwsaResults->lpBlob->pBlobData;

		// get device name
		WCHAR name[256];
		bool bHaveName = pwsaResults->lpszServiceInstanceName && *(pwsaResults->lpszServiceInstanceName);
		StringCchPrintf(name, sizeof(name),L"%s",bHaveName ? pwsaResults->lpszServiceInstanceName : L"");
//        debug(("ServiceInstanceName [%S]", name));
		wstring deviceName((WCHAR *)name, (int)wcslen(name));

        bool paired = FALSE;

		int deviceClass = p_inqRes->classOfDevice;
		if (p_inqRes->flags & BDIF_PAIRED) {
		    paired = TRUE;
		}
		BTH_ADDR deviceAddr;

		deviceAddr = ((SOCKADDR_BTH *)pwsaResults->lpcsaBuffer->RemoteAddr.lpSockaddr)->btAddr;

		// notify listener
        //debug(("doInquiry, notify listener"));
		OnDeviceDiscovered(deviceAddr, deviceClass, deviceName, paired);

		//debug(("doInquiry, listener returns"));
	}

	if (buf != NULL) {
		free(buf);
	}

	if (m_hDeviceLookup != NULL) {
		WSALookupServiceEnd(m_hDeviceLookup);
		m_hDeviceLookup = NULL;
	}

	return result;

}

void CBlueTooth::OnDeviceDiscovered(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired)
{
	//TRACE("%x - %s\n",(unsigned long)deviceAddr, deviceName.c_str());
	//MessageBox(NULL,deviceName.c_str(),0,0);
}

vector<int> CBlueTooth::RunSearchServices(vector<SdpQueryUuid> uuidSet, BTH_ADDR address) 
{
	//debug(("runSearchServices"));
    vector<int> result;


	// 	check if we can handle the number of UUIDs supplied
	if (uuidSet.size() > MAX_UUIDS_IN_QUERY) {
		return result;
	}

	// 	generate a Bluetooth address string (WSAAddressToString doesn't work on WinCE)

	WCHAR addressString[20];

	swprintf_s(addressString, L"(%02x:%02x:%02x:%02x:%02x:%02x)", (int)(address>>40&0xff), (int)(address>>32&0xff), (int)(address>>24&0xff), (int)(address>>16&0xff), (int)(address>>8&0xff), (int)(address&0xff));

	//	build service query

	BTH_QUERY_SERVICE queryservice;

	memset(&queryservice, 0, sizeof(queryservice));

	queryservice.type = SDP_SERVICE_SEARCH_REQUEST;

	for(int i = 0; uuidSet.size(); i++) {

		//UUID is full 128 bits

		queryservice.uuids[i].uuidType = SDP_ST_UUID128;

		queryservice.uuids[i].u.uuid128 = uuidSet[i].u.uuid128;

	}

	// build BLOB pointing to service query

	BLOB blob;

	blob.cbSize = sizeof(queryservice);
	blob.pBlobData = (BYTE *)&queryservice;

	// build query

	WSAQUERYSET queryset;

	memset(&queryset, 0, sizeof(WSAQUERYSET));

	queryset.dwSize = sizeof(WSAQUERYSET);
	queryset.dwNameSpace = NS_BTH;
	queryset.lpBlob = &blob;

	queryset.lpszContext = addressString;

	HANDLE hLookupSearchServices;

	// begin query

	if (WSALookupServiceBegin(&queryset, LUP_FLUSHCACHE, &hLookupSearchServices)) {
		Utils::ShowError(L"RunSearchServices");
		return result;
	}

	// fetch results

	int bufSize = 0x2000;
	void* buf = malloc(bufSize);
	if (buf == NULL) {
		WSALookupServiceEnd(hLookupSearchServices);
		return result;
	}
	memset(buf, 0, bufSize);

	LPWSAQUERYSET pwsaResults = (LPWSAQUERYSET) buf;
	pwsaResults->dwSize = sizeof(WSAQUERYSET);
	pwsaResults->dwNameSpace = NS_BTH;
	pwsaResults->lpBlob = NULL;

	DWORD size = bufSize;


	if (WSALookupServiceNext(hLookupSearchServices, LUP_RETURN_BLOB, &size, pwsaResults)) {
		int last_error = WSAGetLastError();
		switch(last_error) {
			case WSANO_DATA:
				result.clear();
				break;
			default:
				//debug(("WSALookupServiceNext error [%i] %S", last_error, getWinErrorMessage(last_error)));
				result.clear();
		}
	} else {
		// construct int array to hold handles
		result.resize(pwsaResults->lpBlob->cbSize/sizeof(ULONG));
		memcpy(&result[0], pwsaResults->lpBlob->pBlobData, pwsaResults->lpBlob->cbSize);
	}
	WSALookupServiceEnd(hLookupSearchServices);
	free(buf);
	return result;
}

vector<char> CBlueTooth::GetServiceAttributes(vector<int> attrIDs, BTH_ADDR address, int handle)
{
    //debug(("getServiceAttributes"));
	vector<char> result;

	// generate a Bluetooth address string (WSAAddressToString doesn't work on WinCE)

	WCHAR addressString[20];

	swprintf_s(addressString, L"(%02x:%02x:%02x:%02x:%02x:%02x)", (int)(address>>40&0xff), (int)(address>>32&0xff), (int)(address>>24&0xff), (int)(address>>16&0xff), (int)(address>>8&0xff), (int)(address&0xff));

	// build attribute query

	BTH_QUERY_SERVICE *queryservice = (BTH_QUERY_SERVICE *)malloc(sizeof(BTH_QUERY_SERVICE)+sizeof(SdpAttributeRange)*(attrIDs.size()-1));
	memset(queryservice, 0, sizeof(BTH_QUERY_SERVICE)-sizeof(SdpAttributeRange));

	queryservice->type = SDP_SERVICE_ATTRIBUTE_REQUEST;
	queryservice->serviceHandle = handle;
	queryservice->numRange = (ULONG)attrIDs.size();

	// set attribute ranges

	for(unsigned int i = 0; i < attrIDs.size(); i++) {
		queryservice->pRange[i].minAttribute = (USHORT)attrIDs[i];
		queryservice->pRange[i].maxAttribute = (USHORT)attrIDs[i];
	}

	// build BLOB pointing to attribute query

	BLOB blob;

	blob.cbSize = sizeof(BTH_QUERY_SERVICE);

	blob.pBlobData = (BYTE *)queryservice;

	// build query

	WSAQUERYSET queryset;

	memset(&queryset, 0, sizeof(WSAQUERYSET));

	queryset.dwSize = sizeof(WSAQUERYSET);
	queryset.dwNameSpace = NS_BTH;

	queryset.lpszContext = addressString;
	queryset.lpBlob = &blob;

	HANDLE hLookupServiceAttributes;

	// begin query

	if (WSALookupServiceBegin(&queryset, LUP_FLUSHCACHE, &hLookupServiceAttributes)) {
		free(queryservice);
		//throwIOExceptionWSAGetLastError(env, "Failed to begin attribute query");
		Utils::ShowError(L"GetServiceAttributes");
		return result;
	}

	free(queryservice);

	// fetch results
	int bufSize = 0x2000;
	void* buf = malloc(bufSize);
	if (buf == NULL) {
		WSALookupServiceEnd(hLookupServiceAttributes);
		return result;
	}
	memset(buf, 0, bufSize);

	LPWSAQUERYSET pwsaResults = (LPWSAQUERYSET) buf;
	pwsaResults->dwSize = sizeof(WSAQUERYSET);
	pwsaResults->dwNameSpace = NS_BTH;
	pwsaResults->lpBlob = NULL;

	DWORD size = bufSize;

	if (WSALookupServiceNext(hLookupServiceAttributes, LUP_RETURN_BLOB, &size, pwsaResults)) {
		//throwIOExceptionWSAGetLastError(env, "Failed to perform attribute query");
		Utils::ShowError(L"GetServiceAttributes");
		result.clear();
	} else {
		// construct byte array to hold blob
		result.resize(pwsaResults->lpBlob->cbSize);
		memcpy(&result[0], pwsaResults->lpBlob->pBlobData, pwsaResults->lpBlob->cbSize);
	}
	WSALookupServiceEnd(hLookupServiceAttributes);
	free(buf);
	return result;
}



#ifdef UNITTEST
#include "unittest_config.h"
#include "gtest/gtest.h"

CBlueTooth cbt;
SOCKADDR_BTH localBtAddr;

TEST(BlueToothTest,Init)
{
	ASSERT_TRUE(cbt.InitializationStatus());
	ASSERT_TRUE(cbt.GetLocalAddress(localBtAddr));
	wcout<<L"Local BT address is "<<hex<<localBtAddr.btAddr<<endl;
}

#endif