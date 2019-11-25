// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModeBLEP.h"
#include "BLECentral.h"



void AGameModeBLEP::BeginPlay() {
  
  Super::BeginPlay();

  CurrentLowEnergyCentral = NewObject<UBLECentral>();
  TArray<UBLEDevice*> devices =  CurrentLowEnergyCentral->SearchBLEDevices();
  devices[0]->Connect();
  devices[0]->GetCharacteristicValue(devices[0]->deviceCharacteristics[0]);
  devices[1]->Connect();
  devices[1]->GetCharacteristicValue(devices[1]->deviceCharacteristics[4]);
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
