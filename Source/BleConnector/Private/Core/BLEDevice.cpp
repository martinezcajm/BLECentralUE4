// Fill out your copyright notice in the Description page of Project Settings.

#include "BLEDevice.h"

UBLEDevice::UBLEDevice() : UObject() {
  deviceHandle = nullptr;
  pServiceBuffer = nullptr;
  pCharacteristicsBuffer = nullptr;
  ready_ = false;
  path = "";
  friendlyName_ = "";
}

UBLEDevice::UBLEDevice(FString p) {
  path = p;
  friendlyName_ = "";
}

void UBLEDevice::FriendlyName(FString fn) {
  friendlyName_ = fn;
}

void UBLEDevice::CreateHandle() {
  if (path != "") {
    if (deviceHandle != nullptr) CloseHandle(deviceHandle);
    deviceHandle = CreateFile(*path, GENERIC_WRITE | GENERIC_READ, NULL, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (deviceHandle == INVALID_HANDLE_VALUE) {
      //TODO error controlling
      return;
    }
    ready_ = true;
  }
}

void UBLEDevice::BeginDestroy() {
  if (deviceHandle != nullptr) {
    CloseHandle(deviceHandle);
  }
  Super::BeginDestroy();
}

//Get the characteristics of a device
//Based in the example provided by Microsoft at the oficial API
//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattgetservices
void UBLEDevice::TestGetCharacteristics() {
  if (!ready_) return;

  USHORT charBufferSize;

  HRESULT result = BluetoothGATTGetCharacteristics(
    deviceHandle,
    pServiceBuffer,
    0,
    NULL,
    &charBufferSize,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != result) {
    //Error
  }

  //Made a global as we need it to get the characteristics
  //PBTH_LE_GATT_CHARACTERISTIC pCharacteristicsBuffer;
  if (charBufferSize > 0) {
    std::cout << "The device has  " << charBufferSize << " Characteristics." << std::endl;
    pCharacteristicsBuffer = (PBTH_LE_GATT_CHARACTERISTIC)malloc(sizeof(BTH_LE_GATT_CHARACTERISTIC)*charBufferSize);
  }
  if (nullptr == pCharacteristicsBuffer) {
    //Error no more memory
  }

  //We set the initial value of the buffer to 0
  memset(pCharacteristicsBuffer, 0, sizeof(BTH_LE_GATT_CHARACTERISTIC) * charBufferSize);

  //We retrieve the characteristics
  USHORT num_characteristics = 0;
  result = BluetoothGATTGetCharacteristics(
    deviceHandle,
    pServiceBuffer,
    charBufferSize,
    pCharacteristicsBuffer,
    &num_characteristics,
    BLUETOOTH_GATT_FLAG_NONE
  );

  for (USHORT i = 0; i < charBufferSize; i++) {
    GUID guid = (pCharacteristicsBuffer + i)->CharacteristicUuid.Value.LongUuid;

    OLECHAR* guidString;
    StringFromCLSID(guid, &guidString);
    std::cout << " Characteristic " << i << ": Characteristic Short UUID(" << (pCharacteristicsBuffer + i)->CharacteristicUuid.Value.ShortUuid << ") Characteristic Long UUID(" << guidString << ") with attribute handle "
      << (pCharacteristicsBuffer + i)->AttributeHandle << " from service: " << (pCharacteristicsBuffer + i)->ServiceHandle << std::endl;
  }

  if (num_characteristics != charBufferSize) {
    //Missmatch between buffer size and actual buffer size
  }
}

//Get the services of a device
//Similar to get services, an example can alos be found at the oficial API
//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattgetcharacteristics
void UBLEDevice::TestGetGattServices() {

  if (!ready_) return;

  USHORT serviceBufferCount;
  HRESULT result = BluetoothGATTGetServices(
    deviceHandle,
    0,
    NULL,
    &serviceBufferCount,
    BLUETOOTH_GATT_FLAG_NONE
  );
  //The error is not caused by a lack of space which is the
  //one we are expecting after the call
  if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != result) {
    //Error
  }

  std::cout << "The device has  " << serviceBufferCount << " Services." << std::endl;
  //Made a global as we need it to get the characteristics
  //PBTH_LE_GATT_SERVICE pServiceBuffer;
  pServiceBuffer = (PBTH_LE_GATT_SERVICE)malloc(sizeof(BTH_LE_GATT_SERVICE) * serviceBufferCount);
  if (nullptr == pServiceBuffer) {
    //Error no more memory
  }
  //We set the initial value of the buffer to 0
  memset(pServiceBuffer, 0, sizeof(BTH_LE_GATT_SERVICE) * serviceBufferCount);

  //We retrieve the service
  USHORT num_services = 0;
  result = BluetoothGATTGetServices(
    deviceHandle,
    serviceBufferCount,
    pServiceBuffer,
    &num_services,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if (num_services != serviceBufferCount) {
    //Missmatch between buffer size and actual buffer size
  }

  for (USHORT i = 0; i < num_services; i++) {
    GUID guid = (pServiceBuffer + i)->ServiceUuid.Value.LongUuid;

    OLECHAR* guidString;
    StringFromCLSID(guid, &guidString);
    std::cout << " Service " << i << ": Service Short UUID(" << (pServiceBuffer + i)->ServiceUuid.Value.ShortUuid << ") Service Long UUID(" << guidString << ") with attribute handle " << (pServiceBuffer + i)->AttributeHandle << std::endl;
  }
}

//Get the descriptors of a characteristic
//Similar to get services, an example can alos be found at the oficial API
//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattgetdescriptors
void UBLEDevice::TestGetDescriptors() {

  if (!ready_) return;

  USHORT descriptorBufferSize;
  PBTH_LE_GATT_CHARACTERISTIC currGattChar;
  currGattChar = &pCharacteristicsBuffer[2];
  HRESULT result = BluetoothGATTGetDescriptors(
    deviceHandle,
    currGattChar,
    0,
    NULL,
    &descriptorBufferSize,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != result) {
    //Error
    return;
  }

  PBTH_LE_GATT_DESCRIPTOR pDescriptorBuffer = nullptr;

  if (descriptorBufferSize > 0) {
    pDescriptorBuffer = (PBTH_LE_GATT_DESCRIPTOR)malloc(sizeof(BTH_LE_GATT_DESCRIPTOR)*descriptorBufferSize);
  }
  if (nullptr == pCharacteristicsBuffer) {
    //Error no more memory
    return;
  }

  //We set the initial value of the buffer to 0
  memset(pDescriptorBuffer, 0, sizeof(BTH_LE_GATT_DESCRIPTOR) * descriptorBufferSize);

  //We retrieve the descriptors
  USHORT num_descriptors;
  result = BluetoothGATTGetDescriptors(
    deviceHandle,
    pCharacteristicsBuffer,
    descriptorBufferSize,
    pDescriptorBuffer,
    &num_descriptors,
    BLUETOOTH_GATT_FLAG_NONE
  );
  if (num_descriptors != descriptorBufferSize) {
    //Missmatch between buffer size and actual buffer size
  }
}