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

  /*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DeviceHUD")
    UTextBlock *deviceName;
	
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DeviceHUD")
    UTextBlock *connectionStatus;*/
	
};
