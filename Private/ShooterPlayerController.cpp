// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"

#include "Blueprint/UserWidget.h"

AShooterPlayerController::AShooterPlayerController()
{
	
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Check HUD overlay class TSubClassOf Variable has been set in our blueprints through engine

	if(HUDOverlayClass)
	{
		Overlay = CreateWidget(this, HUDOverlayClass);
		if(Overlay)
		{
			Overlay->AddToViewport();
			Overlay->SetVisibility(ESlateVisibility::Visible);
			
			
		}

		
	}
}
