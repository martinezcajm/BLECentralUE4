// Fill out your copyright notice in the Description page of Project Settings.

#include "DemoCharacteristicsShow.h"
#include "UMG/Public/Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "BLEDevice.h"

void UDemoCharacteristicsShow::UpdateCharacteristicsDevice(UBLEDevice* device) {
  CharacteristicsRepresentation->ClearChildren();
  int characteristic_idx = 0;
  if (device == nullptr) return;
  //We connect to the device
  device->Connect();
  for (UGATTCharacteristic* characteristic : device->deviceCharacteristics) {
    UDemoCharacteristicUI* characteristicItemInstance = CreateWidget<UDemoCharacteristicUI>(GetWorld(), GATTCharacteristic);
    //We get the value of the characteristic
    device->GetCharacteristicValue(characteristic);
    characteristicItemInstance->initUI(characteristic, device);
    UUniformGridSlot* DeviceSlot = CharacteristicsRepresentation->AddChildToUniformGrid(characteristicItemInstance);
    DeviceSlot->SetRow(characteristic_idx / MaxColumns);
    DeviceSlot->SetColumn(characteristic_idx%MaxColumns);
    characteristic_idx++;
  }
}

void UDemoCharacteristicsShow::CloseHUD() {
  this->RemoveFromViewport();
}
