// Fill out your copyright notice in the Description page of Project Settings.

#include "BLECentral.h"

TArray<UBLEDevice*> UBLECentral::SearchBLEDevices() {

  TArray<UBLEDevice*> devices;
  
  HDEVINFO hDeviceInfo;

  SP_DEVINFO_DATA deviceInfoData;
  
  DWORD deviceIndex = 0;

  hDeviceInfo = SetupDiGetClassDevs(&GUID_BLUETOOTHLE_DEVICE_INTERFACE, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

  if (hDeviceInfo == INVALID_HANDLE_VALUE) {
    return devices;
  }

  // Enumerate through all devices in Set.
  memset(&deviceInfoData, 0, sizeof(SP_DEVINFO_DATA));
  deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

  while (SetupDiEnumDeviceInfo(hDeviceInfo, deviceIndex, &deviceInfoData)) {
    deviceIndex++;

    DWORD interfaceIndex = 0;

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;

    memset(&deviceInterfaceData, 0, sizeof(SP_DEVICE_INTERFACE_DATA));
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    while (SetupDiEnumDeviceInterfaces(
      hDeviceInfo,
      &deviceInfoData,
      &GUID_BLUETOOTHLE_DEVICE_INTERFACE,
      interfaceIndex,
      &deviceInterfaceData
    )) {
      interfaceIndex++;

      FString path = GetDeviceInterfaceDetail(EDeviceInterfaceDetail::IE_Path, hDeviceInfo,
        deviceInfoData, deviceInterfaceData);
      
      UBLEDevice* newDevice = NewObject<UBLEDevice>();

      FString friendlyName = GetDeviceProperty(EDeviceProperty::PE_FriendlyName, hDeviceInfo, deviceInfoData);
      newDevice->FriendlyName(friendlyName);
      newDevice->Path(path);

      devices.Add(newDevice);
    }
  }

  

  return devices;

}

FString UBLECentral::GetDeviceProperty(EDeviceProperty devProperty, HDEVINFO deviceInfo, SP_DEVINFO_DATA deviceInfoData) {
  DWORD nameData;
  LPSTR nameBuffer = NULL;
  DWORD nameBufferSize = 0;

  SetupDiGetDeviceRegistryProperty(
    deviceInfo,
    &deviceInfoData,
    (DWORD)devProperty,
    NULL,
    NULL,
    0,
    &nameBufferSize
  );

  //We use the TArray from UE4 instead of allocating the memory ourselfs.
  //The TCHAT is to make sure that we are not affected by the encoding of characters
  //In this case is a wide_char
  TArray<TCHAR> value;
  //Wide chars use 2 bytes for one char so we only need half of the required size
  value.AddUninitialized(nameBufferSize * 0.5);

  SetupDiGetDeviceRegistryProperty(
    deviceInfo,
    &deviceInfoData,
    (DWORD)devProperty,
    &nameData,
    (PBYTE)value.GetData(),
    nameBufferSize,
    &nameBufferSize
  );

  FString result = value.GetData();
  return result;
}

FString UBLECentral::GetDeviceInterfaceDetail(EDeviceInterfaceDetail devIntData, HDEVINFO deviceInfo,
  SP_DEVINFO_DATA deviceInfoData, SP_DEVICE_INTERFACE_DATA deviceInterfaceData) {
  DWORD interfaceDataSize = 0;
  //Special call to get the size of the data
  SetupDiGetDeviceInterfaceDetail(
    deviceInfo,
    &deviceInterfaceData,
    NULL,
    NULL,
    &interfaceDataSize,
    NULL);

  PSP_DEVICE_INTERFACE_DETAIL_DATA pDevIntDetData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(
    sizeof(PSP_DEVICE_INTERFACE_DETAIL_DATA) + interfaceDataSize
  );

  memset(pDevIntDetData, 0, sizeof(PSP_DEVICE_INTERFACE_DETAIL_DATA) + interfaceDataSize);
  pDevIntDetData->cbSize = sizeof(PSP_DEVICE_INTERFACE_DETAIL_DATA);
  SetupDiGetDeviceInterfaceDetail(
    deviceInfo,
    &deviceInterfaceData,
    pDevIntDetData,
    interfaceDataSize,
    &interfaceDataSize,
    &deviceInfoData
  );
  FString path = pDevIntDetData->DevicePath;
  int size = pDevIntDetData->cbSize;
  free(pDevIntDetData);

  switch (devIntData) {
  case EDeviceInterfaceDetail::IE_Path:
    return path;
  case EDeviceInterfaceDetail::IE_Size:
    return FString::FromInt(size);
  default:
    return FString();
  }

}
