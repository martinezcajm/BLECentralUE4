// Fill out your copyright notice in the Description page of Project Settings.

#include "DemoBLEItem.h"
#include "DemoCharacteristicsShow.h"
#include "BLEDevice.h"

void UDemoBLEItem::setDevice(UBLEDevice* device) {
  my_device = device;
}

void UDemoBLEItem::CreateCharacteristicWidget() {
  if (HUDCharacteristics != nullptr)
  {
    UDemoCharacteristicsShow* CurrentMenu = CreateWidget<UDemoCharacteristicsShow>(GetWorld(), HUDCharacteristics);
    if (CurrentMenu != nullptr)
    {
      CurrentMenu->AddToViewport();
      CurrentMenu->UpdateCharacteristicsDevice(my_device);
      //CurrentMenu->SetVisibility(ESlateVisibility::Hidden);
    }
  }
}

