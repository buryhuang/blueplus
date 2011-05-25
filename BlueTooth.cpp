/** \addtogroup bluetooth
 *  @{
 */

#include "BlueTooth.h"

#include "Utils.h"

#include <iostream>
#include <algorithm>
using namespace std;

vector<int> CBlueTooth::BTServiceUuid16List;
CBlueTooth* CBlueTooth::m_instance=NULL;

CBlueTooth::CBlueTooth(void):
	m_bBluetoothStackPresent(false),
	m_bStarted(false),
	m_hDeviceLookup(NULL),
	m_bInitialBtIsDiscoverable(false),
	m_bRestoreBtMode(false),
	m_pHandler(NULL)
{
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		WSAGetLastError();
		m_bStarted = false;
	} else {
		m_bStarted = true;
    }

	if(CBlueTooth::BTServiceUuid16List.size()==0){
#if 0
		CBlueTooth::BTServiceUuid16List.push_back(SyncMLClient_UUID16                           );  
		CBlueTooth::BTServiceUuid16List.push_back(ServiceDiscoveryServerServiceClassID_UUID16   );
		CBlueTooth::BTServiceUuid16List.push_back(BrowseGroupDescriptorServiceClassID_UUID16    );
		CBlueTooth::BTServiceUuid16List.push_back(PublicBrowseGroupServiceClassID_UUID16        );
#endif
		CBlueTooth::BTServiceUuid16List.push_back(SerialPortServiceClassID_UUID16               );
#if 0
		CBlueTooth::BTServiceUuid16List.push_back(LANAccessUsingPPPServiceClassID_UUID16        );
		CBlueTooth::BTServiceUuid16List.push_back(DialupNetworkingServiceClassID_UUID16         );
		CBlueTooth::BTServiceUuid16List.push_back(IrMCSyncServiceClassID_UUID16                 );
		CBlueTooth::BTServiceUuid16List.push_back(OBEXObjectPushServiceClassID_UUID16           );
		CBlueTooth::BTServiceUuid16List.push_back(OBEXFileTransferServiceClassID_UUID16         );
		CBlueTooth::BTServiceUuid16List.push_back(IrMcSyncCommandServiceClassID_UUID16          );
		CBlueTooth::BTServiceUuid16List.push_back(HeadsetServiceClassID_UUID16                  );
		CBlueTooth::BTServiceUuid16List.push_back(CordlessServiceClassID_UUID16                 );
		CBlueTooth::BTServiceUuid16List.push_back(AudioSourceServiceClassID_UUID16              );
		CBlueTooth::BTServiceUuid16List.push_back(AudioSinkServiceClassID_UUID16                );
		CBlueTooth::BTServiceUuid16List.push_back(AV_RemoteControlTargetServiceClassID_UUID16   );
		CBlueTooth::BTServiceUuid16List.push_back(AdvancedAudioDistributionServiceClassID_UUID16);
		CBlueTooth::BTServiceUuid16List.push_back(AV_RemoteControlServiceClassID_UUID16         );
		CBlueTooth::BTServiceUuid16List.push_back(VideoConferencingServiceClassID_UUID16        );
		CBlueTooth::BTServiceUuid16List.push_back(IntercomServiceClassID_UUID16                 );
		CBlueTooth::BTServiceUuid16List.push_back(FaxServiceClassID_UUID16                      );
		CBlueTooth::BTServiceUuid16List.push_back(HeadsetAudioGatewayServiceClassID_UUID16      );
		CBlueTooth::BTServiceUuid16List.push_back(PANUServiceClassID_UUID16                     );
		CBlueTooth::BTServiceUuid16List.push_back(NAPServiceClassID_UUID16                      );
		CBlueTooth::BTServiceUuid16List.push_back(GNServiceClassID_UUID16                       );
		CBlueTooth::BTServiceUuid16List.push_back(HandsfreeServiceClassID_UUID16                );
		CBlueTooth::BTServiceUuid16List.push_back(HandsfreeAudioGatewayServiceClassID_UUID16    );
		CBlueTooth::BTServiceUuid16List.push_back(PnPInformationServiceClassID_UUID16           );
		CBlueTooth::BTServiceUuid16List.push_back(GenericNetworkingServiceClassID_UUID16        );
		CBlueTooth::BTServiceUuid16List.push_back(GenericFileTransferServiceClassID_UUID16      );
		CBlueTooth::BTServiceUuid16List.push_back(GenericAudioServiceClassID_UUID16             );
		CBlueTooth::BTServiceUuid16List.push_back(GenericTelephonyServiceClassID_UUID16         );
#endif
		reverse(CBlueTooth::BTServiceUuid16List.begin(),CBlueTooth::BTServiceUuid16List.end());
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
	if(InitializationStatus()==false){
		return false;
	}
	SOCKET s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	//Utils::ShowError(TEXT("isBluetoothStackPresent"));
	if (s == INVALID_SOCKET) {
		int last_error = WSAGetLastError();
		//debug(("socket error [%d] %S", last_error, getWinErrorMessage(last_error)));
		Utils::ShowError(TEXT("isBluetoothStackPresent"));
		return false;
	}

	memset(&btAddr, 0, sizeof(SOCKADDR_BTH));
	btAddr.addressFamily = AF_BTH;

	btAddr.port = BT_PORT_ANY;

	if (bind(s, (SOCKADDR *)&btAddr, sizeof(SOCKADDR_BTH))) {
		int last_error = WSAGetLastError();
		//debug(("bind error [%d] %S", last_error, getWinErrorMessage(last_error)));
		Utils::ShowError(TEXT("isBluetoothStackPresent"));
		closesocket(s);
		return false;
	}

	int size = sizeof(SOCKADDR_BTH);
	if (getsockname(s, (sockaddr*)&btAddr, &size)) {
		int last_error = WSAGetLastError();
//		debug(("getsockname error [%d] %S", last_error, getWinErrorMessage(last_error)));
		closesocket(s);
		return false;
	}
	closesocket(s);
	return true;
}

bool CBlueTooth::IsBluetoothStackPresent() {

	SOCKET s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	//Utils::ShowError(TEXT("isBluetoothStackPresent"));
	if (s == INVALID_SOCKET) {
		int last_error = WSAGetLastError();
		//debug(("socket error [%d] %S", last_error, getWinErrorMessage(last_error)));
		Utils::ShowError(TEXT("isBluetoothStackPresent"));
		return false;
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
		return false;
	}

	int size = sizeof(SOCKADDR_BTH);
	if (getsockname(s, (sockaddr*)&btAddr, &size)) {
		int last_error = WSAGetLastError();
//		debug(("getsockname error [%d] %S", last_error, getWinErrorMessage(last_error)));
		closesocket(s);
		return false;
	}

	closesocket(s);
	//return true;
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
			m_bInitialBtIsDiscoverable = true;
		}
		return true;
    }
    return false;
}

bool CBlueTooth::GetBluetoothGetRadioInfo(BTH_ADDR address, BLUETOOTH_RADIO_INFO* info) {
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
					return true;
				}
			}
		} while( BluetoothFindNextRadio( hFind, &hRadio ) );
		BluetoothFindRadioClose( hFind );
	}
	return false;
}

bool CBlueTooth::getBluetoothDeviceInfo(BTH_ADDR address, BLUETOOTH_DEVICE_INFO* pbtdi, BOOL issueInquiry) {
	BLUETOOTH_DEVICE_SEARCH_PARAMS btsp;
	memset(&btsp, 0, sizeof(btsp));
    btsp.dwSize = sizeof(btsp);
    btsp.fIssueInquiry = issueInquiry;
	btsp.fReturnAuthenticated = true;
    btsp.fReturnConnected     = true;
    btsp.fReturnRemembered    = true;
    btsp.fReturnUnknown       = true;
    if (issueInquiry) {
        btsp.cTimeoutMultiplier = 10;
    }

    memset(pbtdi, 0, sizeof(BLUETOOTH_DEVICE_INFO));
	pbtdi->dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
	HBLUETOOTH_DEVICE_FIND hFind = BluetoothFindFirstDevice(&btsp, pbtdi);
	if (NULL != hFind) {
		do {
			if (pbtdi->Address.ullLong == address) {
			    BluetoothFindDeviceClose(hFind);
				return true;
            }
			printf("found device %i", pbtdi->Address.ullLong);
			memset(pbtdi, 0, sizeof(BLUETOOTH_DEVICE_INFO));
	        pbtdi->dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
		} while (BluetoothFindNextDevice(hFind, pbtdi));
        BluetoothFindDeviceClose(hFind);
	}
	return false;
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
		_stprintf(name, sizeof(name),L"%s",bHaveName ? pwsaResults->lpszServiceInstanceName : L"");
//        debug(("ServiceInstanceName [%S]", name));
		wstring deviceName((WCHAR *)name, (int)wcslen(name));

        bool paired = false;

		int deviceClass = p_inqRes->classOfDevice;
		if (p_inqRes->flags & BDIF_PAIRED) {
		    paired = true;
		}
		BTH_ADDR deviceAddr;

		deviceAddr = ((SOCKADDR_BTH *)pwsaResults->lpcsaBuffer->RemoteAddr.lpSockaddr)->btAddr;

		// notify listener
        //debug(("doInquiry, notify listener"));
		if(m_pHandler != NULL){
			m_pHandler->OnDeviceDiscovered(deviceAddr, deviceClass, deviceName, paired);
		}

#if 0

		vector<SdpQueryUuid> v;
		for(vector<int>::iterator vi=CBlueTooth::BTServiceUuid16List.begin();
			vi!=CBlueTooth::BTServiceUuid16List.end();
			vi++){
			SdpQueryUuid uid;
			uid.uuidType = SDP_ST_UUID16;
			uid.u.uuid16=*vi;
			v.push_back(uid);
		}

		RunSearchServices();
		vector<int> handles = RunSearchServices(v,deviceAddr);

		vector<int> attrs;
		attrs.push_back(0x0000);
		attrs.push_back(0x0001);
		attrs.push_back(0x0002);

		for(vector<int>::iterator vi=handles.begin();vi!=handles.end();vi++){
			GetServiceAttributes(attrs,deviceAddr,*vi);
		}
#endif

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


// callback for BluetoothSdpEnumAttributes()
BOOL __stdcall callback(ULONG uAttribId, LPBYTE pValueStream, ULONG cbStreamSize, LPVOID pvParam)
{
	SDP_ELEMENT_DATA element;
	// Just a verification, uncomment to see the output!!!
	//printf("Callback() uAttribId: %ul\n", uAttribId);
	//printf("Callback() pValueStream: %d\n ", pValueStream);
	//printf("Callback() cbStreamSize: %ul\n ", cbStreamSize);

	if (BluetoothSdpGetElementData(pValueStream,  cbStreamSize, &element) != ERROR_SUCCESS)
	{
		// Just a verification
		//printf("BluetoothSdpGetElementData() failed with error code %ld\n", WSAGetLastError());
		return false;
	}
	else
	{
		// Just a verification
		//printf("BluetoothSdpGetElementData() is OK!\n");
		return true;
	}
}

bool CBlueTooth::RunSearchServices(BTH_ADDR address)
{
	vector<ServiceRecord> serviceList;

	WSADATA m_data;
	SOCKET s;
	WSAPROTOCOL_INFO protocolInfo;
	int protocolInfoSize;
	WSAQUERYSET querySet, *pResults, querySet2;
	HANDLE hLookup, hLookup2;
	int result;
	static int i;
	BYTE buffer[1000];
	BYTE buffer1[2000];
	DWORD bufferLength, flags, addressSize, bufferLength1;
	CSADDR_INFO *pCSAddr;
	BTH_DEVICE_INFO *pDeviceInfo;
	char addressAsString[2000];
	BLOB *pBlob;
	GUID protocol;

	// Load the winsock2 library
	if (WSAStartup(MAKEWORD(2,2), &m_data) == 0)
	{
		//printf("WSAStartup() should be fine!\n");

		// Create a blutooth socket
		s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		if (s == INVALID_SOCKET)
		{
			printf("Failed to get bluetooth socket with error code %ld\n", WSAGetLastError());
			return false;
		}
		else{
			//printf("socket() is OK!\n");
		}

		protocolInfoSize = sizeof(protocolInfo);

		// Get the bluetooth device info using getsockopt()
		if (getsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&protocolInfo, &protocolInfoSize) != 0)
		{
			printf("getsockopt(SO_PROTOCOL_INFO) failed with error code %ld\n", WSAGetLastError());
			return false;
		}
		else{
			//printf("getsockopt(SO_PROTOCOL_INFO) is OK!\n");
		}

		// Query set criteria
		memset(&querySet, 0, sizeof(querySet));
		querySet.dwSize = sizeof(querySet);
		querySet.dwNameSpace = NS_BTH;

		// Set the flags for query
		flags = LUP_RETURN_NAME | LUP_CONTAINERS | LUP_RETURN_ADDR | LUP_FLUSHCACHE |
		LUP_RETURN_TYPE | LUP_RETURN_BLOB | LUP_RES_SERVICE;

		// Start a device in range query...
		result = WSALookupServiceBegin(&querySet, flags, &hLookup);

		// If OK
		if (result == 0)
		{
//			printf("          WSALookupServiceBegin() is OK!\n");
			i = 0;

			while (result == 0)
			{
				bufferLength = sizeof(buffer);
				pResults = (WSAQUERYSET *)&buffer;

				// Next query...
				result = WSALookupServiceNext(hLookup, flags, &bufferLength, pResults);
				if (result != 0)
				{
					printf("          WSALookupServiceNext() failed with error code %ld\n", WSAGetLastError());
				}
				else
				{
					// Get the device info, name, address etc
					//printf("          WSALookupServiceNext() is OK!\n");
					//printf("          The service instance name is %S\n", pResults->lpszServiceInstanceName);
					pCSAddr = (CSADDR_INFO *)pResults->lpcsaBuffer;
					pDeviceInfo = (BTH_DEVICE_INFO *)pResults->lpBlob;
					memset(&querySet2, 0, sizeof(querySet2));
					querySet2.dwSize = sizeof(querySet2);
					protocol = L2CAP_PROTOCOL_UUID;
					querySet2.lpServiceClassId = &protocol;
					querySet2.dwNameSpace = NS_BTH;

					addressSize = sizeof(addressAsString);

					// Print the local bluetooth device address...
					if (WSAAddressToString(pCSAddr->LocalAddr.lpSockaddr, pCSAddr->LocalAddr.iSockaddrLength,
									 &protocolInfo, (LPWSTR)addressAsString, &addressSize) == 0)
					{
					   //printf("          WSAAddressToString() for local address is fine!\n");
					   //printf("          The local address: %S\n", addressAsString);
					}
					else{
					   //printf("          WSAAddressToString() for local address failed with error code %ld\n", WSAGetLastError());
					}

					addressSize = sizeof(addressAsString);

					// Print the remote bluetooth device address...
					if (WSAAddressToString(pCSAddr->RemoteAddr.lpSockaddr, pCSAddr->RemoteAddr.iSockaddrLength,
								&protocolInfo, (LPWSTR)addressAsString, &addressSize) == 0)
					{
						//printf("          WSAAddressToString() for remote address is fine!\n");
						//printf("          The remote device address: %S\n", addressAsString);
					}
					else{
						//printf("          WSAAddressToString() for remote address failed with error code %ld\n", WSAGetLastError());
					}

					if(address ==((SOCKADDR_BTH *)(pCSAddr->RemoteAddr.lpSockaddr))->btAddr)
					{
						// Prepare for service query set
						querySet2.lpszContext = (LPWSTR)addressAsString;

						flags = LUP_FLUSHCACHE |LUP_RETURN_NAME | LUP_RETURN_TYPE |
																 LUP_RETURN_ADDR | LUP_RETURN_BLOB | LUP_RETURN_COMMENT;

						// Start service query
						result = WSALookupServiceBegin(&querySet2, flags, &hLookup2);
						if (result == 0)
						{
							//printf("          WSALookupServiceBegin() is OK!\n");
							while (result == 0)
							{
								bufferLength1 = sizeof(buffer1);
								pResults = (WSAQUERYSET *)&buffer1;

								// Next service query
								result = WSALookupServiceNext(hLookup2, flags, &bufferLength1, pResults);

								if(result == 0)
								{
									ServiceRecord sr;
									// Populate the service info
									//printf("          WSALookupServiceNext() is OK!\n");
									//printf("          WSALookupServiceNext() - service instance name: %S\n",
									//			pResults->lpszServiceInstanceName);
									//printf("          WSALookupServiceNext() - comment (if any): %s\n", pResults->lpszComment);
									//printf("          WSALookupServiceNext() - port (if any): %x\n", ((SOCKADDR_BTH *)pResults->lpcsaBuffer->RemoteAddr.lpSockaddr)->port);
									pCSAddr = (CSADDR_INFO *)pResults->lpcsaBuffer;

									sr.serviceInstanceName=pResults->lpszServiceInstanceName;
									sr.comment=pResults->lpszComment;
									sr.sockaddrBth = *((SOCKADDR_BTH *)pResults->lpcsaBuffer->RemoteAddr.lpSockaddr);

									if(sr.serviceInstanceName.find(L"Serial")!=wstring::npos){
										//TODO hard code for now
										serviceList.push_back(sr);
									}

									// Extract the sdp info
									if (pResults->lpBlob)
									{
										pBlob = (BLOB*)pResults->lpBlob;
										if (!BluetoothSdpEnumAttributes(pBlob->pBlobData, pBlob->cbSize, callback, 0))
										{
											printf("BluetoothSdpEnumAttributes() failed with error code %ld\n", WSAGetLastError());
										}
										else
										{
											//printf("BluetoothSdpEnumAttributes() #%d is OK!\n", i++);
										}
									}
								}
								else
								{
									printf("          WSALookupServiceNext() failed with error code %ld\n", WSAGetLastError());
									printf("          Error code = 11011 ~ WSA_E_NO_MORE ~ No more device!\n");
								}
							}

							// Close the handle to service query
							if(WSALookupServiceEnd(hLookup2) == 0){
								//printf("WSALookupServiceEnd(hLookup2) is fine!\n", WSAGetLastError());
							}else{
								printf("WSALookupServiceEnd(hLookup2) failed with error code %ld\n", WSAGetLastError());
							}
						}
						else {
							printf("WSALookupServiceBegin() failed with error code %ld\n", WSAGetLastError());
							return false;
						}
					}
				}
			}

			// Close handle to the device query
			if(WSALookupServiceEnd(hLookup) == 0){
				//printf("WSALookupServiceEnd(hLookup) is fine!\n", WSAGetLastError());
			}else{
				printf("WSALookupServiceEnd(hLookup) failed with error code %ld\n", WSAGetLastError());
			}
		}
		else
		{
			printf("WSALookupServiceBegin() failed with error code %ld\n", WSAGetLastError());
			return false;
		}// end WSALookupServiceBegin()

		// Cleanup the winsock library startup
		if(WSACleanup() == 0){
			//printf("WSACleanup() pretty fine!\n");
		}else{
			printf("WSACleanup() failed with error code %ld\n", WSAGetLastError());
		}
	} // end WSAStartup()

	if(m_pHandler!=NULL){
		m_pHandler->OnServiceDiscovered(address, serviceList);
	}
	return true;
}

#if 0

vector<int> CBlueTooth::RunSearchServices(vector<SdpQueryUuid> uuidSet, BTH_ADDR address) 
{
	//debug(("runSearchServices"));
    vector<int> result;
	vector<int> finalResult;
	vector<SdpQueryUuid> uuidSetFull=uuidSet;

	// 	check if we can handle the number of UUIDs supplied
	while (uuidSetFull.size() > 0) {
		int maxSize = uuidSetFull.size();
		uuidSet.clear();
		result.clear();
		if(maxSize > MAX_UUIDS_IN_QUERY){
			maxSize = MAX_UUIDS_IN_QUERY;
		}
		for(int i=0;i<maxSize;i++){
			uuidSet.push_back(uuidSetFull.back());
			uuidSetFull.pop_back();
		}

		// 	generate a Bluetooth address string (WSAAddressToString doesn't work on WinCE)
	SOCKADDR_BTH addr;
	memset(&addr, 0, sizeof(SOCKADDR_BTH));

	addr.addressFamily = AF_BTH;
	addr.btAddr = address;
	//addr.port = channel;
		WCHAR addressString[20];
		swprintf_s(addressString, L"(%02x:%02x:%02x:%02x:%02x:%02x)", (int)(address>>40&0xff), (int)(address>>32&0xff), (int)(address>>24&0xff), (int)(address>>16&0xff), (int)(address>>8&0xff), (int)(address&0xff));

		//	build service query

		BTH_QUERY_SERVICE queryservice;
		memset(&queryservice, 0, sizeof(queryservice));
		queryservice.type = SDP_SERVICE_SEARCH_REQUEST;

		for(int i = 0; i<uuidSet.size(); i++) {
			//UUID is full 128 bits
			queryservice.uuids[i].uuidType = SDP_ST_UUID16;
			queryservice.uuids[i].u.uuid16 = uuidSet[i].u.uuid16;
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
			//return result;
			continue;
		}

		// fetch results

		int bufSize = 0x2000;
		void* buf = malloc(bufSize);
		if (buf == NULL) {
			WSALookupServiceEnd(hLookupSearchServices);
			continue;
			//return result;
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

		//merge to final result before next batch
		for(vector<int>::iterator vi=result.begin();vi!=result.end();vi++){
			finalResult.push_back(*vi);
		}
	}
	return finalResult;
}

#endif

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


bool CBlueTooth::RegisterHandler(CBTHandler* pHandler)
{
	if(pHandler!=NULL){
		m_pHandler = pHandler;
		return true;
	}
	return false;
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


/** @}*/
