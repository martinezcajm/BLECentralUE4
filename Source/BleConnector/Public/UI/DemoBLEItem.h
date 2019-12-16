// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMG/Public/Components/TextBlock.h"
#include "DemoBLEItem.generated.h"

/**
 * 
 */
UCLASS()
class BLECONNECTOR_API UDemoBLEItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
  UFUNCTION(BlueprintImplementableEvent, Category = "DeviceItem")
    void SetNameDevice_BP(const FString& connection);

  UFUNCTION(BlueprintImplementableEvent, Category = "DeviceItem")
    void SetConnectionStatus_BP(const FString& name);

  UFUNCTION(BlueprintCallable)
    void setDevice(class UBLEDevice* device);

  UFUNCTION(BlueprintCallable)
    void CreateCharacteristicWidget();

  UPROPERTY(BlueprintReadOnly)
    class UBLEDevice* my_device;

protected:
  //BP instance that we want to use as our Menu
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD", Meta = (BlueprintProtected = "true"))
    TSubclassOf<class UDemoCharacteristicsShow> HUDCharacteristics;


  /*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DeviceHUD")
    UTextBlock *deviceName;
	
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DeviceHUD")
    UTextBlock *connectionStatus;*/
	
};
