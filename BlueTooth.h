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

typedef vector<SdpQueryUuid> UUIDSET;

typedef string BYTEBUFFER;


#define INQUIRY_COMPLETED 0
#define INQUIRY_TERMINATED 5
#define INQUIRY_ERROR 7



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
	vector<int> RunSearchServices(vector<SdpQueryUuid> uuidSet, BTH_ADDR address);
	vector<char> GetServiceAttributes(vector<int> attrIDs, BTH_ADDR address, int handle);

	virtual void OnDeviceDiscovered(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, bool paired);

	bool GetLocalAddress(SOCKADDR_BTH&);

protected:
	bool m_bBluetoothStackPresent;
	HANDLE m_hDeviceLookup;
	bool m_bStarted;
	bool m_bInitialBtIsDiscoverable;
	bool m_bRestoreBtMode;

	bool IsBluetoothStackPresent();
	bool GetBluetoothGetRadioInfo(long address, BLUETOOTH_RADIO_INFO* info);
};

#endif