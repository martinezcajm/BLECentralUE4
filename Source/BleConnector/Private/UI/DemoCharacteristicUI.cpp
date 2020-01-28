// Fill out your copyright notice in the Description page of Project Settings.

#include "DemoCharacteristicUI.h"
#include "GATTCharacteristic.h"
#include "BLEDevice.h"

void UDemoCharacteristicUI::initUI(class UGATTCharacteristic* characteristic, class UBLEDevice* device) {
  my_characteristic = characteristic;
  my_device = device;
  if (characteristic->CanBeWritten()) {
    ShowWriteUI_BP();
  }
  else {
    HideWriteUI_BP();
  }
  
  FString UUI = FString("GUID: ") + characteristic->characteristicGUID();
  FString int_value;
  int_value.AppendInt(characteristic->GetValueAsInt());
  FString value = FString("Value: Int (") + int_value +
    FString(") String ( " + characteristic->GetValueAsString() + FString(")"));
  if (!characteristic->canBeRead()) {
    value = FString("No read characteristic.");
  }
  SetValue_BP(value);
  SetUUI_BP(UUI);
}

void UDemoCharacteristicUI::WriteCharacteristic(const FString& value) {
  my_device->SetCharacteristicValue(my_characteristic, (void*)*value, 3);
}