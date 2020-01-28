// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "bleapi_includes.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GATTCharacteristic.generated.h"

//typedef void(*event_callback)();
//USTRUCT()
//struct FGATTValue {
//  GENERATED_USTRUCT_BODY()
//
//    UPROPERTY()
//    ETypeValue type;
//
//  //TODO this would be better with an union. Unfortunately right now UE4 doesn't support
//  //unions as propperties so can't be used at bluepritns
//  //UPROPERTY()
//  //  typedef union value {
//  //  uint16 ui16;
//  //  uint8 ui8;
//  //  FString s;
//  //} v;
//
//  UPROPERTY()
//    FString s;
//
//  UPROPERTY()
//    uint8 ui8;
//
//};

struct GATTValue {
  uint16 int_value;

  unsigned char* raw_data;

  UPROPERTY()
    FString string_data;

  uint32 size;
};
/**
 * 
 */
UCLASS()
class BLECONNECTOR_API UGATTCharacteristic : public UObject
{
	GENERATED_BODY()
	
public:

  UGATTCharacteristic();

  virtual void BeginDestroy() override;

  UFUNCTION()
    int getValue();

  UFUNCTION()
    bool writeValue();

  UFUNCTION()
    bool canNotify() const;

  UFUNCTION()
    bool canBeRead() const;

  UFUNCTION()
   bool CanBeWritten() const;

  UFUNCTION()
    bool IsNotifying() const;

  UFUNCTION()
   FString characteristicGUID() const;

  uint16_t id() const;

  uint16_t service_id() const;

  void init(const uint16_t id, const uint16_t service_id,
    const bool notify, const bool read, const bool writeresponse, const bool writenoresponse,
    FString guid);

  void setCallback(void(*callback)());

  void setEventHandle(BLUETOOTH_GATT_EVENT_HANDLE event_handle);

  static void GattEventNotificationCallback(BTH_LE_GATT_EVENT_TYPE EventType, PVOID EventOutParameter, PVOID Context);
	
  UFUNCTION()
    void unregisterNotification();

  void updateValue(PBTH_LE_GATT_CHARACTERISTIC_VALUE gatt_value);

  UFUNCTION(BlueprintCallable)
    FString GetValueAsString();

  UFUNCTION(BlueprintCallable)
    int GetValueAsInt();

  unsigned char* GetValueRawData();

  void Reset();

private:

  bool notify_;

  bool read_;

  bool write_with_response_;

  bool write_without_response_;

  bool notifying_;

  FString charGUID_;

  uint16_t id_;

  uint16_t service_id_;

  ////Callback assigned to the characteristic
  void(*event_callback)() = nullptr;

  BLUETOOTH_GATT_EVENT_HANDLE event_notify_handle = nullptr;

  GATTValue characteristic_value;
};
