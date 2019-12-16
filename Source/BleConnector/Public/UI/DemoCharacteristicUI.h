// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoCharacteristicUI.generated.h"

/**
 * 
 */
UCLASS()
class BLECONNECTOR_API UDemoCharacteristicUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
  UFUNCTION(BlueprintImplementableEvent, Category = "CharacteristicItem")
    void SetValue_BP(const FString& connection);

  UFUNCTION(BlueprintImplementableEvent, Category = "CharacteristicItem")
    void SetUUI_BP(const FString& name);

  UFUNCTION(BlueprintImplementableEvent, Category = "CharacteristicItem")
    void HideWriteUI_BP();

  UFUNCTION(BlueprintImplementableEvent, Category = "CharacteristicItem")
    void ShowWriteUI_BP();
  
  UFUNCTION(BlueprintCallable)
    void initUI(class UGATTCharacteristic* characteristic, class UBLEDevice* device);

  UFUNCTION(BlueprintCallable)
    void WriteCharacteristic(const FString& value);

  UPROPERTY(BlueprintReadOnly)
    class UGATTCharacteristic* my_characteristic;

  UPROPERTY(BlueprintReadOnly)
    class UBLEDevice* my_device;
	
	
};
