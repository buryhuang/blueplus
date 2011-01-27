#include "StdAfx.h"
#include "BlueTooth.h"

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

BOOL CBlueTooth::isBluetoothStackPresent() {

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

BOOL CBlueTooth::initializationStatus() {
    if (!m_bBluetoothStackPresent) {
        if (!isBluetoothStackPresent()) {
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

BOOL CBlueTooth::getBluetoothGetRadioInfo(long address, BLUETOOTH_RADIO_INFO* info) {
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

string CBlueTooth::getradioname(long address) {
	BLUETOOTH_RADIO_INFO radioInfo;
	if (getBluetoothGetRadioInfo(address, &radioInfo)) {
		return string((char*)radioInfo.szName, (int) wcslen(radioInfo.szName));
	}
	return NULL;
}

int CBlueTooth::getDeviceVersion(long address)
{
	BLUETOOTH_RADIO_INFO radioInfo;
	if (getBluetoothGetRadioInfo(address, &radioInfo)) {
		return radioInfo.lmpSubversion;
	}
	return -1;
}

int CBlueTooth::getDeviceManufacturer(long address) {
	BLUETOOTH_RADIO_INFO radioInfo;
	if (getBluetoothGetRadioInfo(address, &radioInfo)) {
		return radioInfo.manufacturer;
	}
	return -1;
}

int CBlueTooth::runDeviceInquiry(int duration)
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
		BOOL bHaveName = pwsaResults->lpszServiceInstanceName && *(pwsaResults->lpszServiceInstanceName);
		StringCchPrintf(name, sizeof(name),L"%s",bHaveName ? pwsaResults->lpszServiceInstanceName : L"");
//        debug(("ServiceInstanceName [%S]", name));
		wstring deviceName((WCHAR *)name, (int)wcslen(name));

        BOOL paired = FALSE;

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

void CBlueTooth::OnDeviceDiscovered(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, BOOL paired)
{
	//TRACE("%x - %s\n",(unsigned long)deviceAddr, deviceName.c_str());
	//MessageBox(NULL,deviceName.c_str(),0,0);
}