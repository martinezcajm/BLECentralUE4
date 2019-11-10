// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeBLEP.generated.h"

/**
 * 
 */
UCLASS()
class BLECONNECTOR_API AGameModeBLEP : public AGameModeBase
{
	GENERATED_BODY()
	
public:
  virtual void BeginPlay() override;
	
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BLE")
    TSubclassOf<class UBLECentral> LowEnergyCentral;

  //Instance of our Bluetooth central
  UPROPERTY()
    class UBLECentral* CurrentLowEnergyCentral;

};
