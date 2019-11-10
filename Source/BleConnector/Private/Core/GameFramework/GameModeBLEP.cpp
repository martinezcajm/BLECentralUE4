// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModeBLEP.h"
#include "BLECentral.h"



void AGameModeBLEP::BeginPlay() {
  
  Super::BeginPlay();

  CurrentLowEnergyCentral = NewObject<UBLECentral>();
  TArray<UBLEDevice*> devices =  CurrentLowEnergyCentral->SearchBLEDevices();
  devices[1]->CreateHandle();
  //The order matters!
  devices[1]->TestGetGattServices();
  devices[1]->TestGetCharacteristics();
  devices[1]->TestGetDescriptors();
  
  /*if (LowEnergyCentral != nullptr) {
    CurrentLowEnergyCentral = LowEnergyCentral->GetDefaultObject<UBLECentral>();
    CurrentLowEnergyCentral->SearchBLEDevices();
  }*/
}
