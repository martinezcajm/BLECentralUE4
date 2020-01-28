// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModeBLEP.h"
#include "BLECentral.h"
#include "DemoMainBLEUI.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"

void AGameModeBLEP::BeginPlay() {
  
  Super::BeginPlay();

  if (HUDMenuClass != nullptr)
  {
    CurrentMenu = CreateWidget<UDemoMainBLEUI>(GetWorld(), HUDMenuClass);
    if (CurrentMenu != nullptr)
    {
      CurrentMenu->AddToViewport();
      //CurrentMenu->SetVisibility(ESlateVisibility::Hidden);
    }
  }

  CurrentLowEnergyCentral = NewObject<UBLECentral>();
  CurrentMenu->setBLECentral(CurrentLowEnergyCentral);
  
}