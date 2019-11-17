// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "bleapi_includes.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GATTService.generated.h"

/**
 * 
 */
UCLASS()
class BLECONNECTOR_API UGATTService : public UObject
{
	GENERATED_BODY()

public:
  UGATTService();

  virtual void BeginDestroy() override;

  PBTH_LE_GATT_SERVICE getData() const;

  PBTH_LE_GATT_CHARACTERISTIC getCharacteristicsData() const;

  static void InitServicesData(PBTH_LE_GATT_SERVICE services_data, uint16_t num_services);

  static uint16_t totalServices();

  void Init(const uint16_t id);

  void InitCharacteristics(PBTH_LE_GATT_CHARACTERISTIC characteristics_data);
	
private:
  //Characteristics of the device
  PBTH_LE_GATT_CHARACTERISTIC pCharacteristicsBuffer;

  void Reset();


  uint16_t id_;
};
