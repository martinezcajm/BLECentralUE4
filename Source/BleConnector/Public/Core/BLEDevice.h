// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "bleapi_includes.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BLEDevice.generated.h"


UENUM(BlueprintType)
enum class EDeviceRegistryProperty : uint8 {
  PE_FriendlyName = SPDRP_FRIENDLYNAME UMETA(DisplayName = "Name"),
  PE_ClassGuid = SPDRP_CLASSGUID UMETA(DisplayName = "ClassGuid"),
  PE_TypeDevice = SPDRP_DEVTYPE UMETA(DisplayName = "DeviceType")
};

UENUM(BlueprintType)
enum class EDeviceProperty : uint8 {
  PE_ConnectionStatus = SPDRP_FRIENDLYNAME UMETA(DisplayName = "Connection")
};

UENUM(BlueprintType)
enum class EDeviceInterfaceDetail : uint8 {
  IE_Path UMETA(DisplayName = "Path"),
  IE_Size  UMETA(DisplayName = "ClassGuid")
};

UENUM(BlueprintType)
enum class EReadMode : uint8 {
  RE_None = BLUETOOTH_GATT_FLAG_NONE UMETA(DisplayName = "None"),
  RE_FromDevice = BLUETOOTH_GATT_FLAG_FORCE_READ_FROM_DEVICE UMETA(DisplayName = "Device"),
  RE_FromCache = BLUETOOTH_GATT_FLAG_FORCE_READ_FROM_CACHE UMETA(DisplayName = "Cache")
};

USTRUCT()
struct FGATTValue {
  GENERATED_USTRUCT_BODY()

  UPROPERTY()
    FString s;

  UPROPERTY()
    int int_value;

};

/**
 * 
 */
UCLASS()
class BLECONNECTOR_API UBLEDevice : public UObject
{
  GENERATED_BODY()

public:

  UBLEDevice();

  //UBLEDevice(HDEVINFO info, SP_DEVINFO_DATA infoData, SP_DEVICE_INTERFACE_DATA interfaceData);

  UFUNCTION()
    void FriendlyName(FString fn);

  UFUNCTION()
    void Path(FString p);

  UFUNCTION()
    void Connect();

  UFUNCTION()
    void CreateHandle();

  UFUNCTION(BlueprintCallable)
    bool IsConnected();

  UFUNCTION()
    FGATTValue GetCharacteristicValue(class UGATTCharacteristic* characteristic, EReadMode readMode);

  void SetCharacteristicValue(class UGATTCharacteristic* characteristic, void* data, USHORT charValueDataSize, bool reliable_write = false);

  virtual void BeginDestroy() override;

  void Init(HDEVINFO info, SP_DEVINFO_DATA infoData, SP_DEVICE_INTERFACE_DATA interfaceData);

  FString getFriendlyName() const;
	
private:

  UPROPERTY()
    FString path_;

  UPROPERTY()
    FString friendlyName_;

  UPROPERTY()
    bool ready_;

  FString GetDeviceRegistryProperty(EDeviceRegistryProperty devProperty);

  //Device Propperty can return different kinds of values, for now we are using it to get connection status
  //as this is an int we only treat that right now. Function may need rework if different return options
  //want to be supported.
  uint32_t GetDevicePropertyInt(EDeviceProperty devProperty);

  FString GetDeviceInterfaceDetail(EDeviceInterfaceDetail devIntData);

  //Device info
  HDEVINFO deviceInfo;

  //Device info data
  SP_DEVINFO_DATA deviceInfoData;

  //Device interface detail
  SP_DEVICE_INTERFACE_DATA deviceInterfaceData;

  //Handle to use the device
  HANDLE deviceHandle;

  PBTH_LE_GATT_CHARACTERISTIC_VALUE GetCharacteristicValueIntern(PBTH_LE_GATT_CHARACTERISTIC characteristic, HANDLE service_handle, HRESULT *result, EReadMode readMode);

  PBTH_LE_GATT_SERVICE pServiceBuffer = nullptr;

  //uint16_t numServices = 0;

  ////static function passed to BluetoothGATTRegisterEvent function
  //static void GattEventNotificationCallback(BTH_LE_GATT_EVENT_TYPE EventType, PVOID EventOutParameter, PVOID Context);
  ////current characteristic
  //static uint8_t current_characteristic;
	
public:
  PBTH_LE_GATT_SERVICE GetGattServices(uint16_t* numServices);

  PBTH_LE_GATT_CHARACTERISTIC GetGATTCharacteristics(const class UGATTService* service, uint16_t* numCharacteristics);

  void ActivateNotify(class UGATTCharacteristic& characteristic, void(*callback)());

  void DeactivateNotify(class UGATTCharacteristic& characteristic);

  void Reset();

protected:
  
  void PrepareCharacteristicForNotify(const class UGATTCharacteristic& characteristic);

  PBTH_LE_GATT_DESCRIPTOR getDescriptors(const class UGATTCharacteristic& characteristic, uint16_t *numDescriptors);

  FString ConvertGUIDtoString(const GUID guid) const;

  PBTH_LE_GATT_CHARACTERISTIC getCharacteristicData(const class UGATTCharacteristic& characteristic);

  PBTH_LE_GATT_SERVICE getCharacteristicServiceData(const class UGATTCharacteristic& characteristic);

  HANDLE GetBLEHandle(GUID AGuid);

  GUID ShortUuidToGuid(const USHORT uuid16byte) const;

  GUID GetGuidFromService(const PBTH_LE_GATT_SERVICE service) const;

public:
  UPROPERTY()
    TArray<class UGATTCharacteristic*> deviceCharacteristics;

  UPROPERTY()
    TArray<class UGATTService*> deviceServices;

};
