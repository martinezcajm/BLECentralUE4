// Fill out your copyright notice in the Description page of Project Settings.

#include "BLEDevice.h"
#include "GATTCharacteristic.h"
#include "GATTService.h"
#include "Engine/Engine.h"

//Common data using the Structures of arrays (SOA) layout to have all the data regarding
//the notify callbacks for the characteristics togheter in memory.
//static struct {
//  TArray<void(*)()> callbacks;
//  uint8_t numCharacteristics = 0;
//}CallbackCharacteristicData;

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
    deviceHandle = CreateFile(*path_, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (deviceHandle == INVALID_HANDLE_VALUE) {
      //TODO error controlling
      return;
    }
    ready_ = true;
  }
}

HANDLE UBLEDevice::GetBLEHandle(GUID AGuid) {
  HDEVINFO hDeviceInfo;
  SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
  SP_DEVINFO_DATA deviceInfoData;

  //DWORD deviceIndex = 0;
  GUID BluetoothInterfaceGUID = AGuid;
  HANDLE hComm = NULL;

  hDeviceInfo = SetupDiGetClassDevs(&BluetoothInterfaceGUID, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

  if (hDeviceInfo == INVALID_HANDLE_VALUE) {
    return NULL;
  }

  // Enumerate through all devices in Set.
  memset(&deviceInfoData, 0, sizeof(SP_DEVINFO_DATA));
  deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

  memset(&deviceInterfaceData, 0, sizeof(SP_DEVICE_INTERFACE_DATA));
  deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

  for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDeviceInfo, NULL, &BluetoothInterfaceGUID, i, &deviceInterfaceData); i++) {
    SP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData;

    DeviceInterfaceDetailData.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    DWORD size = 0;

    if (!SetupDiGetDeviceInterfaceDetail(hDeviceInfo, &deviceInterfaceData, NULL, 0, &size, 0))
    {
      int err = GetLastError();

      if (err == ERROR_NO_MORE_ITEMS) break;

      PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)GlobalAlloc(GPTR, size);

      pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

      if (!SetupDiGetDeviceInterfaceDetail(hDeviceInfo, &deviceInterfaceData, pInterfaceDetailData, size, &size, &deviceInfoData))
        break;

      hComm = CreateFile(
        pInterfaceDetailData->DevicePath,
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

      GlobalFree(pInterfaceDetailData);
    }
  
  
  }

  SetupDiDestroyDeviceInfoList(hDeviceInfo);
  return hComm;

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
  ////We get the service that has the raw data of the characteristics
  //UGATTService* s = deviceServices[characteristic.service_id()];
  ////Using the id of the characteristic we access the data of the characteristic
  //currGattChar = &(s->getCharacteristicsData()[characteristic.id()]);

  PBTH_LE_GATT_CHARACTERISTIC currGattChar = getCharacteristicData(characteristic);

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

  PBTH_LE_GATT_SERVICE service = getCharacteristicServiceData(characteristic);

  //We need a handle of the service, for that we need to call the CreateFile with the UUID of the service
  GUID service_GUID = GetGuidFromService(service);

  HANDLE service_handle = GetBLEHandle(service_GUID);


  HRESULT hr;

  for (int i = 0; i < num_descriptors; i++) {
    
    PBTH_LE_GATT_DESCRIPTOR  currGattDescriptor = &pDescriptorBuffer[i];

    USHORT descValueDataSize;
    hr = BluetoothGATTGetDescriptorValue(
      service_handle,
      currGattDescriptor,
      0,
      NULL,
      &descValueDataSize,
      BLUETOOTH_GATT_FLAG_NONE);

    if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
      //printf("BluetoothGATTGetDescriptorValue - Buffer Size %d", hr);
    }

    PBTH_LE_GATT_DESCRIPTOR_VALUE pDescValueBuffer = (PBTH_LE_GATT_DESCRIPTOR_VALUE)malloc(descValueDataSize);

    if (NULL == pDescValueBuffer) {
      //printf("pDescValueBuffer out of memory\r\n");
    }
    else {
      memset(pDescValueBuffer, 0, descValueDataSize);
    }

    hr = BluetoothGATTGetDescriptorValue(
      service_handle,
      currGattDescriptor,
      (ULONG)descValueDataSize,
      pDescValueBuffer,
      NULL,
      BLUETOOTH_GATT_FLAG_NONE);
    if (S_OK != hr) {
      //printf("BluetoothGATTGetDescriptorValue - Actual Data %d", hr);
    }
    //you may also get a descriptor that is read (and not notify) andi am guessing the attribute handle is out of limits
    // we set all descriptors that are notifiable to notify us via IsSubstcibeToNotification
    if (currGattDescriptor->AttributeHandle < 255) {
      BTH_LE_GATT_DESCRIPTOR_VALUE newValue;

      memset(&newValue, 0, sizeof(newValue));

      newValue.DescriptorType = ClientCharacteristicConfiguration;
      newValue.ClientCharacteristicConfiguration.IsSubscribeToNotification = true;

      HRESULT result = BluetoothGATTSetDescriptorValue(
        service_handle,
        &pDescriptorBuffer[i],
        &newValue,
        BLUETOOTH_GATT_FLAG_NONE);
      }
    
    }

}

//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattgetcharacteristicvalue
//notice that this function won't free the memory allocated so its responsability of the one who called
//this function to do it. This function only should be used by the BLEDevice class. Please use the
//GetChatacteristicValue to get the value
PBTH_LE_GATT_CHARACTERISTIC_VALUE UBLEDevice::GetCharacteristicValueIntern(PBTH_LE_GATT_CHARACTERISTIC characteristic, HANDLE service_handle, HRESULT *result) {
  
  USHORT charValueDataSize = 0;
  
  HRESULT hr = BluetoothGATTGetCharacteristicValue(service_handle,
    characteristic,
    0,
    NULL,
    &charValueDataSize,
    BLUETOOTH_GATT_FLAG_NONE
  );
  *result = hr;

  if (HRESULT_FROM_WIN32(ERROR_MORE_DATA) != hr) {
    return nullptr;
  }
  PBTH_LE_GATT_CHARACTERISTIC_VALUE pCharValueBuffer;
  pCharValueBuffer = (PBTH_LE_GATT_CHARACTERISTIC_VALUE)malloc(charValueDataSize);

  if (NULL == pCharValueBuffer) {       
    return pCharValueBuffer;
  }

  memset(pCharValueBuffer, 0, charValueDataSize);

  hr = BluetoothGATTGetCharacteristicValue(service_handle,
    characteristic,
    (ULONG)charValueDataSize,
    pCharValueBuffer,
    NULL,
    BLUETOOTH_GATT_FLAG_NONE);

  *result = hr;

  if (S_OK != hr) {
    return pCharValueBuffer;
  }
  return pCharValueBuffer;
}

//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattgetcharacteristicvalue
FGATTValue UBLEDevice::GetCharacteristicValue(UGATTCharacteristic *characteristic) {
  FGATTValue result;
  memset(&result, 0, sizeof(FGATTValue));
  if (!characteristic->canBeRead()) {
    result.type = ETypeValue::VE_ERROR;
    result.s = FString::Printf(TEXT("Error: this characteristic is specified as no readable"));
    return result;
  }

  USHORT charValueDataSize = 0;

  PBTH_LE_GATT_CHARACTERISTIC current_characteristic = getCharacteristicData(*characteristic);

  PBTH_LE_GATT_SERVICE service = getCharacteristicServiceData(*characteristic);

  //We need a handle of the service, for that we need to call the CreateFile with the UUID of the service
  GUID service_GUID = GetGuidFromService(service);

  HANDLE service_handle = GetBLEHandle(service_GUID);

  HRESULT hr;

  PBTH_LE_GATT_CHARACTERISTIC_VALUE pCharValueBuffer = GetCharacteristicValueIntern(current_characteristic,
    service_handle, &hr);

  if (S_OK != hr) {
    result.type = ETypeValue::VE_ERROR;
    result.s = FString::Printf(TEXT("Error: Problem reading the characteristic BluetoothGATTGetCharacteristicValue"));
    if (pCharValueBuffer != nullptr) free(pCharValueBuffer);
    pCharValueBuffer = nullptr;
    return result;
  }

  result.type = ETypeValue::VE_STRING;
  FString aux;
  FString aux2;
  for (ULONG iii = 0; iii< pCharValueBuffer->DataSize; iii++) {
    aux += FString::Printf(TEXT("%d"), pCharValueBuffer->Data[iii]);
    aux2 += FString::Printf(TEXT("%c"), pCharValueBuffer->Data[iii]);
  }
  result.s = aux2;
  result.ui8 = FCString::Atoi(*aux);
  //memcpy(&(result.s), &aux, aux.Len());
  if(pCharValueBuffer != nullptr) free(pCharValueBuffer);
  pCharValueBuffer = nullptr;


  return result;
}

//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattbeginreliablewrite
//https://docs.microsoft.com/en-us/windows/win32/api/bluetoothleapis/nf-bluetoothleapis-bluetoothgattsetcharacteristicvalue
void UBLEDevice::SetCharacteristicValue(UGATTCharacteristic* characteristic, void* data, USHORT charValueDataSize, bool reliable_write) {

  if (data == nullptr) return;

  if (!characteristic->CanBeWritten()) {
    return;
  }
  HRESULT hr;

  BTH_LE_GATT_RELIABLE_WRITE_CONTEXT ReliableWriteContext = NULL;

  PBTH_LE_GATT_CHARACTERISTIC current_characteristic = getCharacteristicData(*characteristic);
  PBTH_LE_GATT_SERVICE service = getCharacteristicServiceData(*characteristic);
  //We need a handle of the service, for that we need to call the CreateFile with the UUID of the service
  //GUID service_GUID = GetGuidFromService(service);
  //HANDLE service_handle = GetBLEHandle(service_GUID);
  GUID service_GUID;
  if (service->ServiceUuid.IsShortUuid) {
    service_GUID = ShortUuidToGuid(service->ServiceUuid.Value.ShortUuid);
  }
  else {
    service_GUID = service->ServiceUuid.Value.LongUuid;
  }

  HANDLE service_handle = GetBLEHandle(service_GUID);

  if (reliable_write) {
    hr = BluetoothGATTBeginReliableWrite(service_handle,
      &ReliableWriteContext,
      BLUETOOTH_GATT_FLAG_NONE);
    if (!SUCCEEDED(hr)) {
      //Reliable write was not possible, pass the error
    }
  }

  //Maximum size possible, recommendation to avoid problems
  USHORT maxCharValueDataSize = 512;

  PBTH_LE_GATT_CHARACTERISTIC_VALUE newValue;
  newValue = (PBTH_LE_GATT_CHARACTERISTIC_VALUE)malloc(maxCharValueDataSize + sizeof(BTH_LE_GATT_CHARACTERISTIC_VALUE));

  memset(newValue, 0, maxCharValueDataSize + sizeof(BTH_LE_GATT_CHARACTERISTIC_VALUE));

  newValue->DataSize = charValueDataSize;
  memcpy(newValue->Data, (UCHAR*)data, charValueDataSize);

  // Set the new characteristic value
  hr = BluetoothGATTSetCharacteristicValue(service_handle,
    current_characteristic,
    newValue,
    NULL,
    BLUETOOTH_GATT_FLAG_WRITE_WITHOUT_RESPONSE);

  if (S_OK != hr) {
    //Error
    return;
  }

  if (NULL != ReliableWriteContext) {
    hr = BluetoothGATTEndReliableWrite(service_handle,
      ReliableWriteContext,
      BLUETOOTH_GATT_FLAG_NONE);
  }

  //if (newValue != nullptr) free(newValue);

}

PBTH_LE_GATT_CHARACTERISTIC UBLEDevice::getCharacteristicData(const UGATTCharacteristic& characteristic) {
  //We get the service that has the raw data of the characteristics
  UGATTService* service = deviceServices[characteristic.service_id()];

  //Using the id of the characteristic we access the data of the characteristic
  return &(service->getCharacteristicsData()[characteristic.id()]);
}

PBTH_LE_GATT_SERVICE UBLEDevice::getCharacteristicServiceData(const UGATTCharacteristic& characteristic) {
  //We get the service that has the raw data of the characteristics
  UGATTService* service = deviceServices[characteristic.service_id()];

  return service->getData();
}

GUID UBLEDevice::ShortUuidToGuid(const USHORT uuid16byte) const {
  //To convert a shourt UUID (16 bits) we need to use the BASE bluetooth UUID 00000000-0000-1000-8000-00805F9B34FB
  //bthledef has it already defined it.
  //The UUID of the shourt UUID will follow the next format: 0000xxxx-0000-1000-8000-00805F9B34FB
  //where xxxx is the 16 bits of the short UUID
  GUID Bluetooth_base_UUID = BTH_LE_ATT_BLUETOOTH_BASE_GUID;
  //Data1 are the first 32 bits of our guid so there is where our shrt UUID goes
  Bluetooth_base_UUID.Data1 = uuid16byte;
  return Bluetooth_base_UUID;
}

GUID UBLEDevice::GetGuidFromService(const PBTH_LE_GATT_SERVICE service) const {
  GUID service_GUID;
  if (service->ServiceUuid.IsShortUuid) {
    service_GUID = ShortUuidToGuid(service->ServiceUuid.Value.ShortUuid);
  }
  else {
    service_GUID = service->ServiceUuid.Value.LongUuid;
  }
  return service_GUID;
}

//void UBLEDevice::GattEventNotificationCallback(BTH_LE_GATT_EVENT_TYPE EventType, PVOID EventOutParameter, PVOID Context) {
//
//}

void UBLEDevice::ActivateNotify(class UGATTCharacteristic& characteristic, void(*callback)()) {
  if (characteristic.canNotify()) {
    
    PrepareCharacteristicForNotify(characteristic);
    PBTH_LE_GATT_CHARACTERISTIC current_characteristic = getCharacteristicData(characteristic);

    PBTH_LE_GATT_SERVICE service = getCharacteristicServiceData(characteristic);

    //We need a handle of the service, for that we need to call the CreateFile with the UUID of the service
    GUID service_GUID = GetGuidFromService(service);

    HANDLE service_handle = GetBLEHandle(service_GUID);

    BTH_LE_GATT_EVENT_TYPE EventType = CharacteristicValueChangedEvent;

    BLUETOOTH_GATT_VALUE_CHANGED_EVENT_REGISTRATION EventParameterIn;
    EventParameterIn.Characteristics[0] = *current_characteristic;
    EventParameterIn.NumCharacteristics = 1;

    //TODO will need to go to UGATTCharacteristic probably
    BLUETOOTH_GATT_EVENT_HANDLE EventHandle;

    characteristic.setCallback(callback);

    HRESULT hr = BluetoothGATTRegisterEvent(
      service_handle,
      EventType,
      &EventParameterIn,
      &UGATTCharacteristic::GattEventNotificationCallback,
      &characteristic,
      &EventHandle,
      BLUETOOTH_GATT_FLAG_NONE);

  }
  else {
    //Characteristic can't be notified
  }
}