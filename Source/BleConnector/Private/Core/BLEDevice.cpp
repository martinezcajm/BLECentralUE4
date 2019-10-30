// Fill out your copyright notice in the Description page of Project Settings.

#include "BLEDevice.h"

UBLEDevice::UBLEDevice() : UObject() {
  deviceHandle = nullptr;
  pServiceBuffer = nullptr;
  pCharacteristicsBuffer = nullptr;
  path = "";
  friendlyName = "";
}

UBLEDevice::UBLEDevice(FString p) {
  path = p;
  friendlyName = "";
}

void UBLEDevice::FriendlyName(FString fn) {
  friendlyName = fn;
}

void UBLEDevice::CreateHandle() {
  if (path != "") {
    if (deviceHandle != nullptr) CloseHandle(deviceHandle);
    deviceHandle = CreateFile(*path, GENERIC_WRITE | GENERIC_READ, NULL, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (deviceHandle == INVALID_HANDLE_VALUE) {
      //TODO error controlling
      return;
    }
  }
}

void UBLEDevice::BeginDestroy() {
  if (deviceHandle != nullptr) {
    CloseHandle(deviceHandle);
  }
}


