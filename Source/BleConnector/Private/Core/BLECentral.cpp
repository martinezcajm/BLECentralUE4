// Fill out your copyright notice in the Description page of Project Settings.

#include "BLECentral.h"

TArray<UBLEDevice*> UBLECentral::SearchBLEDevices() {

  TArray<UBLEDevice*> paths;
  
  HDEVINFO hDeviceInfo;

  SP_DEVINFO_DATA deviceInfoData;
  
  DWORD deviceIndex = 0;

  hDeviceInfo = SetupDiGetClassDevs(&GUID_BLUETOOTHLE_DEVICE_INTERFACE, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

  if (hDeviceInfo == INVALID_HANDLE_VALUE) {
    return paths;
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

      DWORD interfaceDataSize = 0;
      //Special call to get the size of the data
      SetupDiGetDeviceInterfaceDetail(
        hDeviceInfo,
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
        hDeviceInfo,
        &deviceInterfaceData,
        pDevIntDetData,
        interfaceDataSize,
        &interfaceDataSize,
        &deviceInfoData
      );
      FString path = pDevIntDetData->DevicePath;
      UBLEDevice newDevice(path);
      paths.Add(&newDevice);

      free(pDevIntDetData);
    }
  }

  DWORD nameData;
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

  nameBuffer = new TCHAR[nameBufferSize * 2];

  SetupDiGetDeviceRegistryProperty(
    hDeviceInfo,
    &deviceInfoData,
    SPDRP_FRIENDLYNAME,
    &nameData,
    (PBYTE)nameBuffer,
    nameBufferSize,
    &nameBufferSize
  );

  return paths;

}


