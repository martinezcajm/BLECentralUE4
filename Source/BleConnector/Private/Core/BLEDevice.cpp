// Fill out your copyright notice in the Description page of Project Settings.

#include "BLEDevice.h"
#include "GATTCharacteristic.h"
#include "GATTService.h"
#include "Engine/Engine.h"

UBLEDevice::UBLEDevice() : UObject() {
  deviceHandle = nullptr;
  //pServiceBuffer = nullptr;
  //pCharacteristicsBuffer = nullptr;
  ready_ = false;
  path_ = "";
  friendlyName_ = "";
}

void UBLEDevice::Init(HDEVINFO info, SP_DEVINFO_DATA infoData, SP_DEVICE_INTERFACE_DATA interfaceData) {
  deviceInfo = info;
  deviceInfoData = infoData;
  deviceInterfaceData = interfaceData;

  path_ = GetDeviceInterfaceDetail(EDeviceInterfaceDetail::IE_Path);

  friendlyName_ = GetDeviceRegistryProperty(EDeviceRegistryProperty::PE_FriendlyName);
}

void UBLEDevice::Connect() {
  CreateHandle();

  uint16_t num_services;
  PBTH_LE_GATT_SERVICE services = GetGattServices(&num_services);
  UGATTService::InitServicesData(services, num_services);

  for (int i = 0; i < num_services; i++) {
    UGATTService* newService = NewObject<UGATTService>();
    newService->Init(i);

    uint16_t num_characteristics;
    PBTH_LE_GATT_CHARACTERISTIC service_characteristics = GetGATTCharacteristics(newService, &num_characteristics);
    for (int j = 0; j < num_characteristics; j++) {
      UGATTCharacteristic* newCharacteristic = NewObject<UGATTCharacteristic>();
      //Unreal treats the warning (C4800) as an error so we need to cast BOOl to bool manualy
      //error C4800: 'BOOLEAN': forcing value to bool 'true' or 'false' (performance warning)
      bool notifable = (service_characteristics[j].IsNotifiable != FALSE);
      bool readable = (service_characteristics[j].IsReadable != FALSE);
      bool writable = (service_characteristics[j].IsWritable != FALSE);
      bool writablewithoutresponse = (service_characteristics[j].IsWritableWithoutResponse != FALSE);

      //We transform the hexadecimal data to string
      //Unreal allows to make an string based in the value of a printf 
      //FString test = FString::Printf(TEXT("GUID: %#010x"), service_characteristics[j].CharacteristicUuid.Value.LongUuid.Data1);
      FString guid = ConvertGUIDtoString(service_characteristics[j].CharacteristicUuid.Value.LongUuid);
      newCharacteristic->init(j, i, notifable, readable, writable, writablewithoutresponse, guid);
      deviceCharacteristics.Add(newCharacteristic);
    }
    newService->InitCharacteristics(service_characteristics);
    deviceServices.Add(newService);
  }
}

FString UBLEDevice::ConvertGUIDtoString(const GUID guid) const {
  FString result;
  result = FString::Printf(TEXT("%#010x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x"),
    guid.Data1, guid.Data2, guid.Data3,
    guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
    guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
  return result;
}

void UBLEDevice::Path(FString p) {
  path_ = p;
}

void UBLEDevice::FriendlyName(FString fn) {
  friendlyName_ = fn;
}

void UBLEDevice::CreateHandle() {
  if (path_ != "") {
    if (deviceHandle != nullptr) CloseHandle(deviceHandle);
    deviceHandle = CreateFile(*path_, GENERIC_WRITE | GENERIC_READ, NULL, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (deviceHandle == INVALID_HANDLE_VALUE) {
      //TODO error controlling
      return;
    }
    ready_ = true;
  }
}

bool UBLEDevice::IsConnected() {
  uint32_t connection = GetDevicePropertyInt(EDeviceProperty::PE_ConnectionStatus);
  if (connection &DISPLAY_DEVICE_DISCONNECT) {
    return false;
  }
  return true;
}

void UBLEDevice::Reset() {
  if (deviceHandle != nullptr) {
    CloseHandle(deviceHandle);
  }
  if (deviceInfo != nullptr) {
    SetupDiDestroyDeviceInfoList(deviceInfo);
  }
}

void UBLEDevice::BeginDestroy() {
  Reset();
  Super::BeginDestroy();
}

FString UBLEDevice::GetDeviceRegistryProperty(EDeviceRegistryProperty devProperty) {
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

uint32_t UBLEDevice::GetDevicePropertyInt(EDeviceProperty devProperty) {
  DWORD requiredLength;
  DEVPROPTYPE prop_type;

  BOOL success = SetupDiGetDeviceProperty(
    deviceInfo,
    &deviceInfoData,
    &DEVPKEY_Device_DevNodeStatus,
    &prop_type,
    NULL,
    0,
    &requiredLength,
    0
  );

  if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
  {
    //error
  }

  //We use the TArray from UE4 instead of allocating the memory ourselfs.
  //TArray<uint8_t> value;
  //
  //value.AddUninitialized(requiredLength);
  ULONG value;

  DWORD actual_length = requiredLength;

  SetupDiGetDeviceProperty(
    deviceInfo,
    &deviceInfoData,
    &DEVPKEY_Device_DevNodeStatus,
    &prop_type,
    (PBYTE)&value,
    actual_length,
    &requiredLength,
    0
  );

  if (actual_length != requiredLength) {
    //error
  }

  return value;
}

FString UBLEDevice::GetDeviceInterfaceDetail(EDeviceInterfaceDetail devIntData) {
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

//Get the characteristics of a device
//Based in the example provided by Microsoft at the oficial API
//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattgetservices
PBTH_LE_GATT_CHARACTERISTIC UBLEDevice::GetGATTCharacteristics(const class UGATTService* service, uint16_t* numCharacteristics) {
  
  PBTH_LE_GATT_CHARACTERISTIC pCharacteristicsBuffer = nullptr;
  
  if (!ready_) return pCharacteristicsBuffer;

  USHORT charBufferSize;

  HRESULT result = BluetoothGATTGetCharacteristics(
    deviceHandle,
    service->getData(),
    0,
    NULL,
    &charBufferSize,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != result) {
    //Error
  }

  
  if (charBufferSize > 0) {
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("The device has %i Characteristics"), charBufferSize));
    //std::cout << "The device has  " << charBufferSize << " Characteristics." << std::endl;
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
    service->getData(),
    charBufferSize,
    pCharacteristicsBuffer,
    &num_characteristics,
    BLUETOOTH_GATT_FLAG_NONE
  );

  for (USHORT i = 0; i < charBufferSize; i++) {
    GUID guid = (pCharacteristicsBuffer + i)->CharacteristicUuid.Value.LongUuid;
    //printf("\n Characteristic %i with GUID: %#010x \n", i, guid.Data1);
    std::cout << " with attribute handle "
      << (pCharacteristicsBuffer + i)->AttributeHandle << " from service: " << (pCharacteristicsBuffer + i)->ServiceHandle << std::endl;
  }
  *numCharacteristics = num_characteristics;
  if (num_characteristics != charBufferSize) {
    //Missmatch between buffer size and actual buffer size
    *numCharacteristics = 0;
  }

  return pCharacteristicsBuffer;
}

//Get the services of a device
//Similar to get services, an example can alos be found at the oficial API
//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattgetcharacteristics
PBTH_LE_GATT_SERVICE UBLEDevice::GetGattServices(uint16_t* numServices) {

  PBTH_LE_GATT_SERVICE pServiceBuffer = nullptr;

  if (!ready_) return pServiceBuffer;

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
  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("The device has %i Services"), serviceBufferCount));
  //std::cout << "The device has  " << serviceBufferCount << " Services." << std::endl;
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
  *numServices = num_services;
  if (num_services != serviceBufferCount) {
    //Missmatch between buffer size and actual buffer size
    *numServices = 0;
  }

  for (USHORT i = 0; i < num_services; i++) {
    GUID guid = (pServiceBuffer + i)->ServiceUuid.Value.LongUuid;
    //printf("\n Service %i with GUID: %#010x \n", i, guid.Data1);
    std::cout << "with attribute handle " << (pServiceBuffer + i)->AttributeHandle << std::endl;
  }

  return pServiceBuffer;
}

//Get the descriptors of a characteristic
//Similar to get services, an example can alos be found at the oficial API
//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattgetdescriptors

PBTH_LE_GATT_DESCRIPTOR UBLEDevice::getDescriptors(const UGATTCharacteristic& characteristic, uint16_t *numDescriptors) {
  
  PBTH_LE_GATT_DESCRIPTOR pDescriptorBuffer = nullptr;

  if (!ready_) return pDescriptorBuffer;

  USHORT descriptorBufferSize;
  PBTH_LE_GATT_CHARACTERISTIC currGattChar;
  //We get the service that has the raw data of the characteristics
  UGATTService* s = deviceServices[characteristic.service_id()];
  //Using the id of the characteristic we access the data of the characteristic
  currGattChar = &(s->getCharacteristicsData()[characteristic.id()]);
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
    return pDescriptorBuffer;
  }

  if (descriptorBufferSize > 0) {
    pDescriptorBuffer = (PBTH_LE_GATT_DESCRIPTOR)malloc(sizeof(BTH_LE_GATT_DESCRIPTOR)*descriptorBufferSize);
  }
  if (nullptr == pDescriptorBuffer) {
    //Error no more memory
    return pDescriptorBuffer;
  }

  //We set the initial value of the buffer to 0
  memset(pDescriptorBuffer, 0, sizeof(BTH_LE_GATT_DESCRIPTOR) * descriptorBufferSize);

  //We retrieve the descriptors
  USHORT num_descriptors;
  result = BluetoothGATTGetDescriptors(
    deviceHandle,
    currGattChar,
    descriptorBufferSize,
    pDescriptorBuffer,
    &num_descriptors,
    BLUETOOTH_GATT_FLAG_NONE
  );
  *numDescriptors = num_descriptors;
  if (num_descriptors != descriptorBufferSize) {
    //Missmatch between buffer size and actual buffer size
    *numDescriptors = 0;
  }

  return pDescriptorBuffer;
  
}

//To be able to get notify we need to suscribe to an event setting the Descriptor as Microsoft 
//indicates us
//https://docs.microsoft.com/es-es/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattsetdescriptorvalue?redirectedfrom=MSDN
void UBLEDevice::PrepareCharacteristicForNotify(const UGATTCharacteristic& characteristic) {

  uint16_t num_descriptors;
  PBTH_LE_GATT_DESCRIPTOR pDescriptorBuffer = getDescriptors(characteristic, &num_descriptors);

  for (int i = 0; i < num_descriptors; i++) {
    BTH_LE_GATT_DESCRIPTOR_VALUE newValue;

    memset(&newValue, 0, sizeof(newValue));

    newValue.DescriptorType = ClientCharacteristicConfiguration;
    newValue.ClientCharacteristicConfiguration.IsSubscribeToNotification = true;

    HRESULT result = BluetoothGATTSetDescriptorValue(deviceHandle,
      &pDescriptorBuffer[i],
      &newValue,
      BLUETOOTH_GATT_FLAG_NONE);
  }
}