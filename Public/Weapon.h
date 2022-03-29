// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class MASTERSHOOTERCOURSE_API AWeapon : public AItem
{
	GENERATED_BODY()


public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;


	// Adds an impulse to the weapon when dropped
	void ThrowWeapon();

	FORCEINLINE int32 GetAmmo() const {return AmmoCount ;}

	// Called from main character class when firing weapon
	void DecreaseAmmo();
	


protected:
	
	
	void StopFalling();

	
private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	// Ammo count for this weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess=true))
	int32 AmmoCount;


};
