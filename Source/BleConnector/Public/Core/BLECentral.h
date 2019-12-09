// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "setupapi_includes.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BLEDevice.h"

#include "BLECentral.generated.h"

/**
 * 
 */
UCLASS()
class BLECONNECTOR_API UBLECentral : public UObject
{
	GENERATED_BODY()

public:
  UFUNCTION(BlueprintCallable)
    TArray<UBLEDevice*> SearchBLEDevices();
	
  UPROPERTY(BlueprintReadOnly)
    TArray<UBLEDevice*> devices;
};
