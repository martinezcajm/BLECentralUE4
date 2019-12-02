// Fill out your copyright notice in the Description page of Project Settings.

#include "DemoMainBLEUI.h"
#include "UMG/Public/Components/UniformGridPanel.h"
#include "UMG/Public/Components/Button.h"
#include "UMG/Public/Components/CanvasPanel.h"
#include "BLECentral.h"

void UDemoMainBLEUI::UpdateDevices() {
  BLERepresentation->ClearChildren();
}
