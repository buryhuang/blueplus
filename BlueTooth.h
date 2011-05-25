/** \addtogroup bluetooth
 *  @{
 */

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include "version.h"

//#include <windows.h>

#include <winsock2.h>
#include <ws2bth.h>
#include <bluetoothapis.h>

#include <string>
#include <vector>
using namespace std;

#ifdef _UNICODE
#define _T(x)     L ## x
#define _tcslen   wcslen
#define _stprintf swprintf
#define _tstoi    _wtoi
#define _tcscmp   wcscmp

#else /* _UNICODE */

#define _T(x)     x
#define _tcslen   strlen
#define _stprintf sprintf
#define _tstoi    atoi
#define _tcscmp   strcmp

#endif /* _UNICODE */

typedef vector<SdpQueryUuid> UUIDSET;

typedef string BYTEBUFFER;


#define INQUIRY_COMPLETED 0
#define INQUIRY_TERMINATED 5
#define INQUIRY_ERROR 7


//BT stack uuid
#define SyncMLClient_UUID16                             0x0002
#define ServiceDiscoveryServerServiceClassID_UUID16     0x1000
#define BrowseGroupDescriptorServiceClassID_UUID16      0x1001
#define PublicBrowseGroupServiceClassID_UUID16          0x1002
#define SerialPortServiceClassID_UUID16                 0x1101
#define LANAccessUsingPPPServiceClassID_UUID16          0x1102
#define DialupNetworkingServiceClassID_UUID16           0x1103
#define IrMCSyncServiceClassID_UUID16                   0x1104
#define OBEXObjectPushServiceClassID_UUID16             0x1105
#define OBEXFileTransferServiceClassID_UUID16           0x1106
#define IrMcSyncCommandServiceClassID_UUID16            0x1107
#define HeadsetServiceClassID_UUID16                    0x1108
#define CordlessServiceClassID_UUID16                   0x1109
#define AudioSourceServiceClassID_UUID16                0x110A
#define AudioSinkServiceClassID_UUID16                  0x110B
#define AV_RemoteControlTargetServiceClassID_UUID16     0x110C
#define AdvancedAudioDistributionServiceClassID_UUID16  0x110D
#define AV_RemoteControlServiceClassID_UUID16           0x110E
#define VideoConferencingServiceClassID_UUID16          0x110F
#define IntercomServiceClassID_UUID16                   0x1110
#define FaxServiceClassID_UUID16                        0x1111
#define HeadsetAudioGatewayServiceClassID_UUID16        0x1112
#define PANUServiceClassID_UUID16                       0x1115
#define NAPServiceClassID_UUID16                        0x1116
#define GNServiceClassID_UUID16                         0x1117
#define HandsfreeServiceClassID_UUID16                  0x111E
#define HandsfreeAudioGatewayServiceClassID_UUID16      0x111F
#define PnPInformationServiceClassID_UUID16             0x1200
#define GenericNetworkingServiceClassID_UUID16          0x1201
#define GenericFileTransferServiceClassID_UUID16        0x1202
#define GenericAudioServiceClassID_UUID16               0x1203
#define GenericTelephonyServiceClassID_UUID16           0x1204

struct ServiceRecord
{
	unsigned int uuid16;
	wstring serviceInstanceName;
	wstring comment;
	SOCKADDR_BTH sockaddrBth;
};

class CBTHandler
{
public:
	CBTHandler(void){};
	virtual ~CBTHandler(void){};

	virtual void OnDeviceDiscovered(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired)=0;
	virtual void OnServiceDiscovered(BTH_ADDR deviceAddr, vector<ServiceRecord>)=0;

};

#define DEF_BTDEVICE CBlueTooth::GetInstance()

class CBlueTooth
{
public:
	CBlueTooth(void);
	virtual ~CBlueTooth(void);

	bool InitializationStatus();
	string GetRadioName(long address);
	int GetDeviceVersion(long address);
	int GetDeviceManufacturer(long address);
	
	int RunDeviceInquiry(int duration);
	bool RunSearchServices(BTH_ADDR address);
	vector<char> GetServiceAttributes(vector<int> attrIDs, BTH_ADDR address, int handle);
	bool RegisterHandler(CBTHandler* pHandler);

	static bool GetBluetoothGetRadioInfo(BTH_ADDR address, BLUETOOTH_RADIO_INFO* info);
	static bool CBlueTooth::getBluetoothDeviceInfo(BTH_ADDR address, BLUETOOTH_DEVICE_INFO* pbtdi, BOOL issueInquiry);


	//virtual void OnDeviceDiscovered(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired);
	//virtual void OnServiceDiscovered(BTH_ADDR deviceAddr, vector<ServiceRecord>);

	bool GetLocalAddress(SOCKADDR_BTH&);

	static vector<int> BTServiceUuid16List;

	static CBlueTooth* GetInstance(){
		if(m_instance==NULL){
			m_instance = new CBlueTooth();
		}
		return m_instance;
	}

protected:
	bool m_bBluetoothStackPresent;
	HANDLE m_hDeviceLookup;
	bool m_bStarted;
	bool m_bInitialBtIsDiscoverable;
	bool m_bRestoreBtMode;

	bool IsBluetoothStackPresent();
	CBTHandler* m_pHandler;
	static CBlueTooth* m_instance;

};

#endif


/** @}*/
