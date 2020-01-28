// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeEmego.generated.h"

/**
 * 
 */
UCLASS()
class BLECONNECTOR_API AGameModeEmego : public AGameModeBase
{
	GENERATED_BODY()
	
public:
  virtual void BeginPlay();

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BLE")
    TSubclassOf<class UBLECentral> LowEnergyCentral;

  //Instance of our Bluetooth central
  UPROPERTY()
    class UBLECentral* CurrentLowEnergyCentral;

  UFUNCTION()
    void TestGetEmegoRawData();

  UFUNCTION()
    void TestGetTwoEmegoRawData();
	
	
};
