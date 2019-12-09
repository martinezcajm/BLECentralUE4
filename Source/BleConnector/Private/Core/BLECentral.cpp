// Fill out your copyright notice in the Description page of Project Settings.

#include "BLECentral.h"

TArray<UBLEDevice*> UBLECentral::SearchBLEDevices() {

  devices.Empty();
  //TArray<UBLEDevice*> devices;
  
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
      
      UBLEDevice* newDevice = NewObject<UBLEDevice>();
      newDevice->Init(hDeviceInfo, deviceInfoData, deviceInterfaceData);

      devices.Add(newDevice);
    }
  }

  return devices;

}