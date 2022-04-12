// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"



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

	virtual void SetItemProperties(EItemState State) override;

	UFUNCTION()
	void AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;
	

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Ammo", meta = (AllowPrivateAccess=true))
	UStaticMeshComponent* AmmoMesh;

	// Decides if the gun will fire this ammo or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Ammo", meta = (AllowPrivateAccess=true))
	EAmmoType AmmoType;

	// The texture used for the ammo icon, depending on which ammo type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Ammo", meta = (AllowPrivateAccess=true))
	UTexture2D* AmmoIconTexture;

	// Overlap Sphere for picking up ammo automatically when running over it
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="My Stuff | Ammo", meta = (AllowPrivateAccess=true))
	class USphereComponent* AmmoCollisionSphere;
	
	
public:

	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh ; }
	FORCEINLINE EAmmoType GetAmmoType() const {	return AmmoType ; }

	
	
};
