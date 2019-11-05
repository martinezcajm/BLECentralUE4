// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma comment(lib, "SetupAPI")
#pragma comment(lib, "BluetoothApis.lib")
#pragma warning( disable : 4068 )

#define NTDDI_VERSION NTDDI_WIN8
#define _WIN32_WINNT _WIN32_WINNT_WIN8


//Unreal need the redefinition of the setup api macros

//Redefinition used to indicate windows platform
//https://docs.microsoft.com/en-us/windows/win32/api/setupapi/ns-setupapi-sp_altplatform_info_v2
// V1 is for windows lesser than XP and V2 from XP to Vista
#define USE_SP_ALTPLATFORM_INFO_V1 0
#define USE_SP_ALTPLATFORM_INFO_V3 1

//Define this identifier only if your component must run on Windows 98 or Millennium Edition, or on Windows NT. 
//https://docs.microsoft.com/en-us/windows/win32/api/setupapi/ns-setupapi-sp_drvinfo_data_v1_w
#define USE_SP_DRVINFO_DATA_V1 0

//TODO no info could be found about this two, for now set to 0 as V1 is usually for lower windows version
//observing the others
#define USE_SP_BACKUP_QUEUE_PARAMS_V1 0
#define USE_SP_INF_SIGNER_INFO_V1 0


#include "AllowWindowsPlatformTypes.h"
#include "Windows/COMPointer.h"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <bthdef.h>
#include <bluetoothleapis.h>

#include <iostream>
#include <sstream>
#include <string>
#include <locale>
////GUID_DEVINTERFACE_* values
#include <uuids.h>


#include "HideWindowsPlatformTypes.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BLEDevice.h"

#include "BLECentral.generated.h"

UENUM(BlueprintType)
enum class EDeviceProperty : uint8 {
  PE_FriendlyName = SPDRP_FRIENDLYNAME UMETA(DisplayName = "Name"),
  PE_ClassGuid = SPDRP_CLASSGUID UMETA(DisplayName = "ClassGuid"),
  PE_TypeDevice = SPDRP_DEVTYPE UMETA(DisplayName = "DeviceType")
};

UENUM(BlueprintType)
enum class EDeviceInterfaceDetail : uint8 {
  IE_Path UMETA(DisplayName = "Path"),
  IE_Size  UMETA(DisplayName = "ClassGuid")
};

/**
 * 
 */
UCLASS()
class BLECONNECTOR_API UBLECentral : public UObject
{
	GENERATED_BODY()

public:
  UFUNCTION()
    TArray<UBLEDevice*> SearchBLEDevices();
	
private:

    FString GetDeviceProperty(EDeviceProperty devProperty, HDEVINFO deviceInfo, SP_DEVINFO_DATA deviceInfoData);

    FString GetDeviceInterfaceDetail(EDeviceInterfaceDetail devIntData, HDEVINFO deviceInfo,
      SP_DEVINFO_DATA deviceInfoData, SP_DEVICE_INTERFACE_DATA deviceInterfaceData);
	
};
