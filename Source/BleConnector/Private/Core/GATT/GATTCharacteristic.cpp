// Fill out your copyright notice in the Description page of Project Settings.

#include "GATTCharacteristic.h"


UGATTCharacteristic::UGATTCharacteristic() : UObject() {
  notify_ = false;
  read_ = false;
  write_with_response_ = false;
  write_without_response_ = false;
  id_ = 0;
  charGUID_ = "";

  characteristic_value.int_value = 0;
  characteristic_value.raw_data = nullptr;
  characteristic_value.size = 0;
  characteristic_value.string_data = FString("");
}

void UGATTCharacteristic::BeginDestroy() {
  Reset();
  Super::BeginDestroy();
}

void UGATTCharacteristic::Reset() {
  if (characteristic_value.raw_data != nullptr) {
    free(characteristic_value.raw_data);
    characteristic_value.raw_data = nullptr;
  }
  unregisterNotification();
}

int UGATTCharacteristic::getValue() {
  return 1;
}

bool UGATTCharacteristic::writeValue() {
  return true;
}

bool UGATTCharacteristic::canNotify() const {
  return notify_;
}

bool UGATTCharacteristic::canBeRead() const {
  return read_;
}

bool UGATTCharacteristic::CanBeWritten() const {
  return write_with_response_ || write_without_response_;
}

FString UGATTCharacteristic::characteristicGUID() const {
  return charGUID_;
}

uint16_t UGATTCharacteristic::id() const {
  return id_;
}

uint16_t UGATTCharacteristic::service_id() const {
  return service_id_;
}

void UGATTCharacteristic::init(const uint16_t id, const uint16_t service_id,
  const bool notify, const bool read, const bool writeresponse, const bool writenoresponse,
  FString guid) {
  id_ = id;
  service_id_ = service_id;
  notify_ = notify;
  read_ = read;
  write_with_response_ = writeresponse;
  write_without_response_ = writenoresponse;
  charGUID_ = guid;
}

//Fix for the crash after more than 1 callback error as Microsoft indicates
//https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/e30e43cf-4cda-40ab-9075-0e1babeaf23f/program-crashed-after-recived-bluetooth-le-notification?forum=wdk&prof=required
void CALLBACK UGATTCharacteristic::GattEventNotificationCallback(BTH_LE_GATT_EVENT_TYPE EventType, PVOID EventOutParameter, PVOID Context) {
  //We need to get the object from the context
  UGATTCharacteristic* pThis = static_cast<UGATTCharacteristic*>(Context);
  
  //We update the value of the characteristic
  PBLUETOOTH_GATT_VALUE_CHANGED_EVENT ValueChangedEventParameters = (PBLUETOOTH_GATT_VALUE_CHANGED_EVENT)EventOutParameter;
  pThis->updateValue(ValueChangedEventParameters->CharacteristicValue);
  
  
  //Once we have it if a callback has been assigned to the characteristic we call it
  if (pThis->event_callback != nullptr) {
    pThis->event_callback();
  }
}

void UGATTCharacteristic::setCallback(void(*callback)()) {
  event_callback = callback;
}

void UGATTCharacteristic::setEventHandle(BLUETOOTH_GATT_EVENT_HANDLE event_handle) {
  event_notify_handle = event_handle;
}

void UGATTCharacteristic::unregisterNotification() {
  if (event_notify_handle != nullptr) {
    HRESULT hr = BluetoothGATTUnregisterEvent(event_notify_handle, BLUETOOTH_GATT_FLAG_NONE);
  }
}

void UGATTCharacteristic::updateValue(PBTH_LE_GATT_CHARACTERISTIC_VALUE gatt_value) {

  //We don't have the value or the size is new (this last case shouldn't happen)
  if (characteristic_value.size == 0 || characteristic_value.size != gatt_value->DataSize) { //We don't have the value or the size is new (this last case shouldn't happen)
    characteristic_value.size = gatt_value->DataSize;
    if (characteristic_value.raw_data != nullptr) {
      free(characteristic_value.raw_data);
    }
    characteristic_value.raw_data = (unsigned char*)malloc(characteristic_value.size);
  }
  //Now we only do this if it's the first time the characteristic is read or the size has
  //changed 
  /*characteristic_value.size = gatt_value->DataSize;
  characteristic_value.raw_data = (unsigned char*)malloc(characteristic_value.size);*/
  //We initialize with the end of string character
  memset(characteristic_value.raw_data, '\0', characteristic_value.size);
  memcpy(characteristic_value.raw_data, gatt_value->Data, characteristic_value.size);
  
  characteristic_value.string_data.Empty();
  //characteristic_value.string_data = FString("");
  for (ULONG iii = 0; iii< characteristic_value.size; iii++) {
    characteristic_value.string_data += FString::Printf(TEXT("%c"), characteristic_value.raw_data[iii]);
  }
  characteristic_value.int_value = (uint16)*(characteristic_value.raw_data);
}

FString UGATTCharacteristic::GetValueAsString() {
  return characteristic_value.string_data;
}

int UGATTCharacteristic::GetValueAsInt() {
  return characteristic_value.int_value;
}