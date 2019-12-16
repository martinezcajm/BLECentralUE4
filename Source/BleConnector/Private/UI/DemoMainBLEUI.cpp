// Fill out your copyright notice in the Description page of Project Settings.

#include "DemoMainBLEUI.h"
#include "UMG/Public/Components/UniformGridPanel.h"
#include "UMG/Public/Components/Button.h"
#include "UMG/Public/Components/CanvasPanel.h"
#include "UMG/Public/Components/TextBlock.h"
#include "Components/UniformGridSlot.h"
#include "BLECentral.h"
#include "BLEDevice.h"

void UDemoMainBLEUI::setBLECentral(UBLECentral * logic) {
  centralLogic = logic;
}

void UDemoMainBLEUI::ScanDevices() {
  if (centralLogic != nullptr) centralLogic->SearchBLEDevices();
  UpdateDevices();
}

void UDemoMainBLEUI::UpdateDevices() {
  BLERepresentation->ClearChildren();
  if (centralLogic != nullptr && !centralLogic->devices.Num()) return;
  int device_idx = 0;

  for (UBLEDevice* device : centralLogic->devices) {
    UDemoBLEItem* deviceItemInstance = CreateWidget<UDemoBLEItem>(GetWorld(), BLEDevices);
    //
    FString device_name = device->getFriendlyName();
    deviceItemInstance->SetNameDevice_BP(device_name);
    deviceItemInstance->setDevice(device);
    //deviceItemInstance->deviceName->SetText(FText::FromString(device_name));
    if (device->IsConnected()) {
      deviceItemInstance->SetConnectionStatus_BP(FString("Connected"));
    }
    else {
      deviceItemInstance->SetConnectionStatus_BP(FString("Not connected"));
    }
    UUniformGridSlot* DeviceSlot = BLERepresentation->AddChildToUniformGrid(deviceItemInstance);
    DeviceSlot->SetRow(device_idx / MaxColumns);
    DeviceSlot->SetColumn(device_idx%MaxColumns);
    device_idx++;
  }
}

