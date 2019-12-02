// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModeBLEP.h"
#include "BLECentral.h"
#include "DemoMainBLEUI.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"


void DoSomething() {
  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Something happened")));
}

void AGameModeBLEP::BeginPlay() {
  
  Super::BeginPlay();

  if (HUDMenuClass != nullptr)
  {
    CurrentMenu = CreateWidget<UDemoMainBLEUI>(GetWorld(), HUDMenuClass);
    if (CurrentMenu != nullptr)
    {
      CurrentMenu->AddToViewport();
      //CurrentMenu->SetVisibility(ESlateVisibility::Hidden);
    }
  }

  CurrentLowEnergyCentral = NewObject<UBLECentral>();
  TArray<UBLEDevice*> devices =  CurrentLowEnergyCentral->SearchBLEDevices();
  //devices[0]->Connect();
  //devices[0]->GetCharacteristicValue(devices[0]->deviceCharacteristics[0]);
  devices[1]->Connect();
  devices[1]->GetCharacteristicValue(devices[1]->deviceCharacteristics[0]);
  devices[1]->GetCharacteristicValue(devices[1]->deviceCharacteristics[1]);
  devices[1]->GetCharacteristicValue(devices[1]->deviceCharacteristics[2]);
  devices[1]->GetCharacteristicValue(devices[1]->deviceCharacteristics[3]);
  devices[1]->GetCharacteristicValue(devices[1]->deviceCharacteristics[4]);
  //TEST sending data to Emego (write)
  //Byte 0 (OP code) byte 1 (data) byte 2 (Data)
  //For this case OP code = 0 (Trigger level)
  // Byte 2 = 8
  // Byte 3 = 42
  // So to summarize we are sending the value 2090 to the Emego trigger level (goes from 0 to 4095)
  byte data[] = {0x00, 0x08, 0x2A};
  devices[1]->SetCharacteristicValue(devices[1]->deviceCharacteristics[5], data, 3);
  devices[1]->ActivateNotify(*(devices[1]->deviceCharacteristics[4]), &DoSomething);
  //devices[0]->CreateHandle();
  //The order matters!
  /*devices[1]->TestGetGattServices();
  devices[1]->TestGetCharacteristics();
  devices[1]->TestGetDescriptors();*/
  //bool status = devices[0]->IsConnected();
  //if (status) {
  //  devices[0]->GetGattServices();
  //  devices[0]->GetGATTCharacteristics();
  //  devices[0]->TestGetDescriptors();
  //}
  
}