// Fill out your copyright notice in the Description page of Project Settings.

#include "GATTCharacteristic.h"


UGATTCharacteristic::UGATTCharacteristic() : UObject() {
  notify_ = false;
  read_ = false;
  write_ = false;
  id_ = 0;
  charGUID_ = "";
}

int UGATTCharacteristic::getValue() {
  return 1;
}

bool UGATTCharacteristic::writeValue() {
  return true;
}

bool UGATTCharacteristic::canNotify() const {
  return notify_;
}

bool UGATTCharacteristic::canBeRead() const {
  return read_;
}

bool UGATTCharacteristic::CanBeWritten() const {
  return write_;
}

FString UGATTCharacteristic::characteristicGUID() const {
  return charGUID_;
}

uint16_t UGATTCharacteristic::id() const {
  return id_;
}

uint16_t UGATTCharacteristic::service_id() const {
  return service_id_;
}

void UGATTCharacteristic::init(const uint16_t id, const uint16_t service_id, const bool notify, const bool read, const bool write) {
  id_ = id;
  service_id_ = service_id;
  notify_ = notify;
  read_ = read;
  write_ = write;
}
