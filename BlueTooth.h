#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H

#include "version.h"

#include <windows.h>

#include <winsock2.h>
#include <ws2bth.h>
#include <bluetoothapis.h>

#include "Utils.h"

#include <string>
#include <vector>
using namespace std;

typedef vector<SdpQueryUuid> UUIDSET;


#define INQUIRY_COMPLETED 0
#define INQUIRY_TERMINATED 5
#define INQUIRY_ERROR 7



class CBlueTooth
{
public:
	CBlueTooth(void);
	virtual ~CBlueTooth(void);

	BOOL initializationStatus();
	string getradioname(long address);
	int getDeviceVersion(long address);
	int getDeviceManufacturer(long address);
	int runDeviceInquiry(int duration);

	virtual void OnDeviceDiscovered(BTH_ADDR deviceAddr, int deviceClass, wstring deviceName, BOOL paired);

protected:
	BOOL m_bBluetoothStackPresent;
	HANDLE m_hDeviceLookup;
	BOOL m_bStarted;
	BOOL m_bInitialBtIsDiscoverable;
	BOOL m_bRestoreBtMode;

	BOOL isBluetoothStackPresent();
	BOOL getBluetoothGetRadioInfo(long address, BLUETOOTH_RADIO_INFO* info);
};

#endif