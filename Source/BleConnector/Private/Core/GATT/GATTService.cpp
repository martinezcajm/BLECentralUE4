// Fill out your copyright notice in the Description page of Project Settings.


#include "GATTService.h"

//Common data using the Structures of arrays (SOA) layout to have all the data regarding
//the services togheter in memory. This way we also make use of the base data structure 
//of windows PBTH_LE_GATT_SERVICE
static struct {
  PBTH_LE_GATT_SERVICE pServiceBuffer = nullptr;
  uint8_t numServices = 0;
}ServicesData;

void UGATTService::InitServicesData(PBTH_LE_GATT_SERVICE services_data, uint16_t num_services) {
  ServicesData.pServiceBuffer = services_data;
  ServicesData.numServices = num_services;
}

uint16_t UGATTService::totalServices() {
  return ServicesData.numServices;
}

UGATTService::UGATTService() : UObject() {
  pCharacteristicsBuffer = nullptr;
  id_ = 0;
}

void UGATTService::BeginDestroy() {
  Reset();
  Super::BeginDestroy();
}

void UGATTService::Reset() {
  if (pCharacteristicsBuffer != nullptr) {
    free(pCharacteristicsBuffer);
    pCharacteristicsBuffer = nullptr;
  }
  if (ServicesData.pServiceBuffer != nullptr) {
    free(ServicesData.pServiceBuffer);
    ServicesData.pServiceBuffer = nullptr;
  }
}

PBTH_LE_GATT_SERVICE UGATTService::getData() const {
  return &(ServicesData.pServiceBuffer[id_]);
}

PBTH_LE_GATT_CHARACTERISTIC UGATTService::getCharacteristicsData() const {
  return pCharacteristicsBuffer;
}

void UGATTService::Init(const uint16_t id) {
  id_ = id;
}

void UGATTService::InitCharacteristics(PBTH_LE_GATT_CHARACTERISTIC characteristics_data) {
  pCharacteristicsBuffer = characteristics_data;
}