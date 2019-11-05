// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BLECentral.h"
#include "BLE_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLECONNECTOR_API UBLE_GameInstance : public UGameInstance
{
  GENERATED_BODY()
public:

    UBLE_GameInstance();

    UPROPERTY()
      TSubclassOf<class UBLECentral> LowEnergyCentral;

    //Instance of our Bluetooth central
    UPROPERTY()
      class UBLECentral* CurrentLowEnergyCentral;
	
	
};
