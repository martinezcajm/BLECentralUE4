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

      //DWORD interfaceDataSize = 0;
      ////Special call to get the size of the data
      //SetupDiGetDeviceInterfaceDetail(
      //  hDeviceInfo,
      //  &deviceInterfaceData,
      //  NULL,
      //  NULL,
      //  &interfaceDataSize,
      //  NULL);

      //PSP_DEVICE_INTERFACE_DETAIL_DATA pDevIntDetData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(
      //  sizeof(PSP_DEVICE_INTERFACE_DETAIL_DATA) + interfaceDataSize
      //);

      //memset(pDevIntDetData, 0, sizeof(PSP_DEVICE_INTERFACE_DETAIL_DATA) + interfaceDataSize);
      //pDevIntDetData->cbSize = sizeof(PSP_DEVICE_INTERFACE_DETAIL_DATA);
      //SetupDiGetDeviceInterfaceDetail(
      //  hDeviceInfo,
      //  &deviceInterfaceData,
      //  pDevIntDetData,
      //  interfaceDataSize,
      //  &interfaceDataSize,
      //  &deviceInfoData
      //);
      //FString path = pDevIntDetData->DevicePath;
      FString path = GetDeviceInterfaceDetail(EDeviceInterfaceDetail::IE_Path, hDeviceInfo,
        deviceInfoData, deviceInterfaceData);
      UBLEDevice newDevice(path);
      

      //free(pDevIntDetData);

      /*DWORD nameData;
      LPSTR nameBuffer = NULL;
      DWORD nameBufferSize = 0;

      SetupDiGetDeviceRegistryProperty(
        hDeviceInfo,
        &deviceInfoData,
        SPDRP_FRIENDLYNAME,
        NULL,
        NULL,
        0,
        &nameBufferSize
      );

      nameBuffer = (LPSTR)new TCHAR[nameBufferSize * 2];

      SetupDiGetDeviceRegistryProperty(
        hDeviceInfo,
        &deviceInfoData,
        SPDRP_FRIENDLYNAME,
        &nameData,
        (PBYTE)nameBuffer,
        nameBufferSize,
        &nameBufferSize
      );
      FString friendlyName = nameBuffer; */
      FString friendlyName = GetDeviceProperty(EDeviceProperty::PE_FriendlyName, hDeviceInfo, deviceInfoData);
      newDevice.FriendlyName(friendlyName);

      devices.Add(&newDevice);
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

  nameBuffer = (LPSTR)new TCHAR[nameBufferSize * 2];

  SetupDiGetDeviceRegistryProperty(
    deviceInfo,
    &deviceInfoData,
    (DWORD)devProperty,
    &nameData,
    (PBYTE)nameBuffer,
    nameBufferSize,
    &nameBufferSize
  );
  FString result = nameBuffer;
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
