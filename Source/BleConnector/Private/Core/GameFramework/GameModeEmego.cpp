// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModeEmego.h"
#include "BLECentral.h"
#include "GATTCharacteristic.h"
#include "Engine/Engine.h"

static TArray<UBLEDevice*> devices;

void ParseEmegoData(unsigned char* emego_data) {
  //mask to get the integrity count from Emego every 2 bytes the 4 upper bits
  //so this mask gets them with an & (1111 0000)
  const uint8 integrity_count_mask = 0xF0;
  //mask to get the value
  const uint16 value_mask = 0x0FFF;

  //Every emego package comes with 10 values, every one stored in two bytes
  uint8 integrity_count = 0;
  uint16 value = 0;

  //for (int i = 0; i < 10; i++) {
  //  //Every iteration treats a pair of bytes from the array
  //  //First we need to get the integrity count from the first byte of the pair.
  //  integrity_count = (emego_data[i*2] & integrity_count_mask) >> 4;
  //  //We need to concatenate the pair in a uint16 so we convert the More
  //  //Significative byte (first of the pair) to an uint16 and move it 8 bits to the left
  //  //Then we add the Less Significative byte
  //  value = ((uint16)emego_data[i * 2] << 8) | (emego_data[(i*2) + 1]);

  //  //We need to remove the integrity count
  //  value = value & value_mask;
  //  UE_LOG(LogTemp, Log, TEXT("Value number %i : integrity count [%i] - value[%i])"), i+1, integrity_count, value);
  //}
  for (int i = 0; i < 10; i++) {
    //Every iteration treats a pair of bytes from the array
    //First we need to get the integrity count from the first byte of the pair.
    integrity_count = (emego_data[(i * 2) + 1] & integrity_count_mask) >> 4;
    //We need to concatenate the pair in a uint16 so we convert the More
    //Significative byte (first of the pair) to an uint16 and move it 8 bits to the left
    //Then we add the Less Significative byte
    value = ((uint16)emego_data[(i * 2) + 1] << 8) | (emego_data[i*2]);

    //We need to remove the integrity count
    value = value & value_mask;
    UE_LOG(LogTemp, Log, TEXT("Value number %i : integrity count [%i] - value[%i])"), i+1, integrity_count, value);
  }
}

void EmegoRawData() {
  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Something happened")));
  unsigned char* data = devices[0]->deviceCharacteristics[7]->GetValueRawData();
  UE_LOG(LogTemp, Log, TEXT("Emego Device 1: raw data collected: %s"), data);
  ParseEmegoData(data);
}

void EmegoRawData2() {
  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Something happened 2")));
  unsigned char* data = devices[1]->deviceCharacteristics[7]->GetValueRawData();
  UE_LOG(LogTemp, Log, TEXT("Emego Device 2: raw data collected: %s"), data);
  ParseEmegoData(data);
}

void AGameModeEmego::BeginPlay() {

  Super::BeginPlay();
  CurrentLowEnergyCentral = NewObject<UBLECentral>();
  //TestGetEmegoRawData();
  TestGetTwoEmegoRawData();
  
}

void AGameModeEmego::TestGetEmegoRawData() {
  //TArray<UBLEDevice*> devices = CurrentLowEnergyCentral->SearchBLEDevices();
  devices = CurrentLowEnergyCentral->SearchBLEDevices();
  //devices[0]->Connect();
  //devices[0]->GetCharacteristicValue(devices[0]->deviceCharacteristics[0]);
  devices[0]->Connect();
  devices[0]->GetCharacteristicValue(devices[0]->deviceCharacteristics[0], EReadMode::RE_FromDevice);
  devices[0]->GetCharacteristicValue(devices[0]->deviceCharacteristics[1], EReadMode::RE_FromDevice);
  devices[0]->GetCharacteristicValue(devices[0]->deviceCharacteristics[2], EReadMode::RE_FromDevice);
  devices[0]->GetCharacteristicValue(devices[0]->deviceCharacteristics[3], EReadMode::RE_FromDevice);
  devices[0]->GetCharacteristicValue(devices[0]->deviceCharacteristics[4], EReadMode::RE_FromDevice);
  //TEST sending data to Emego (write)
  //Byte 0 (OP code) byte 1 (data) byte 2 (Data)
  //For this case OP code = 0 (Trigger level)
  // Byte 2 = 8
  // Byte 3 = 42
  // So to summarize we are sending the value 2090 to the Emego trigger level (goes from 0 to 4095)
  //byte data[] = {0x00, 0x08, 0x2A};
  //TEST sending data to Emego (write)
  //Byte 0 (OP code) byte 1 (data) byte 2 (Data)
  //For this case OP code = 0 (Trigger level)
  // Byte 2 = 8
  // Byte 3 = 42
  // So to summarize we are sending the value 2090 to the Emego trigger level (goes from 0 to 4095)
  byte data[] = { 0x07, 0x00, 0x01 };
  devices[0]->SetCharacteristicValue(devices[0]->deviceCharacteristics[5], data, 3);
  //devices[1]->ActivateNotify(*(devices[1]->deviceCharacteristics[4]), &DoSomething);
  devices[0]->ActivateNotify(*(devices[0]->deviceCharacteristics[7]), &EmegoRawData);
}

void AGameModeEmego::TestGetTwoEmegoRawData() {
  //TArray<UBLEDevice*> devices = CurrentLowEnergyCentral->SearchBLEDevices();
  devices = CurrentLowEnergyCentral->SearchBLEDevices();
  byte data[] = { 0x07, 0x00, 0x01 };
  //devices[0]->Connect();
  //devices[0]->GetCharacteristicValue(devices[0]->deviceCharacteristics[0]);
  devices[0]->Connect();

  devices[0]->SetCharacteristicValue(devices[0]->deviceCharacteristics[5], data, 3);
  //devices[1]->ActivateNotify(*(devices[1]->deviceCharacteristics[4]), &DoSomething);
  devices[0]->ActivateNotify(*(devices[0]->deviceCharacteristics[7]), &EmegoRawData);

  devices[1]->Connect();

  devices[1]->SetCharacteristicValue(devices[1]->deviceCharacteristics[5], data, 3);
  //devices[1]->ActivateNotify(*(devices[1]->deviceCharacteristics[4]), &DoSomething);
  devices[1]->ActivateNotify(*(devices[1]->deviceCharacteristics[7]), &EmegoRawData2);

  
}
