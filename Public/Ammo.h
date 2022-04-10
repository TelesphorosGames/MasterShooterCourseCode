// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class MASTERSHOOTERCOURSE_API AAmmo : public AItem
{
	GENERATED_BODY()
	
public:

AAmmo();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Ammo", meta = (AllowPrivateAccess=true))
	UStaticMeshComponent* AmmoMesh;

public:

	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh ; }

	
	
};
