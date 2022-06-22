// Fill out your copyright notice in the Description page of Project Settings.


#include "FletchedPlayerController.h"
#include "Blueprint/UserWidget.h"

void AFletchedPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* HUD = CreateWidget(this, HUDClass);
	if(HUD != nullptr)
	{
		HUD->AddToViewport();	
	}
}
