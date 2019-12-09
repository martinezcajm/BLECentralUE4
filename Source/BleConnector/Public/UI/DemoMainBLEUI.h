// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DemoBLEItem.h"
#include "DemoMainBLEUI.generated.h"

/**
 * 
 */
UCLASS()
class BLECONNECTOR_API UDemoMainBLEUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

  UFUNCTION(BlueprintCallable)
    void UpdateDevices();

  void setBLECentral(class UBLECentral * logic);

protected:
  UFUNCTION(BlueprintCallable)
    void ScanDevices();
  
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BLEHUD")
    class UCanvasPanel *MenuPanel;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ScannButton")
    class UButton *ScannButton;
	
  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BLEDevices")
    class UUniformGridPanel *BLERepresentation;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BLEDevices")
    TSubclassOf<UDemoBLEItem> BLEDevices;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BLEDevices/Parameters")
    int MaxRow = 2;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BLEDevices/Parameters")
    int MaxColumns = 3;

  UPROPERTY()
    class UBLECentral *centralLogic = nullptr;

};
