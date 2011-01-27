#include "com_intel_bluetooth_BluetoothStackMicrosoft.h"

typedef int (WINAPI * DLL_getLibraryVersion)();

typedef BOOL (WINAPI * DLL_initializationStatus)();