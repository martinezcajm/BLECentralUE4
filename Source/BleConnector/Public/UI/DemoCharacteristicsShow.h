// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoCharacteristicUI.h"
#include "DemoCharacteristicsShow.generated.h"

/**
 * 
 */
UCLASS()
class BLECONNECTOR_API UDemoCharacteristicsShow : public UUserWidget
{
	GENERATED_BODY()
	
public:
  UFUNCTION(BlueprintCallable)
    void UpdateCharacteristicsDevice(class UBLEDevice* device);

  UFUNCTION(BlueprintCallable)
    void CloseHUD();
	
protected:
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BLEDevices")
    class UUniformGridPanel *CharacteristicsRepresentation;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BLEDevices")
    TSubclassOf<UDemoCharacteristicUI> GATTCharacteristic;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BLEDevices")
    int MaxRow = 4;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BLEDevices")
    int MaxColumns = 3;
	
};
