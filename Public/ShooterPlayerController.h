// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MASTERSHOOTERCOURSE_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	AShooterPlayerController();
private:


	//Reference to the Overall HUD overlay blueprint class : Will provide a drop-down in the engine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Widgets", meta=(AllowPrivateAccess=true))
	TSubclassOf<class UUserWidget> HUDOverlayClass;

	// Space in memory for Overlay Variable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Widgets", meta=(AllowPrivateAccess=true))
	UUserWidget* Overlay;

protected:

	virtual void BeginPlay() override;
};
