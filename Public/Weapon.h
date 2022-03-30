// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),

	EWT_MAX UMETA(DisplayName ="DefaultMAX")
};
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
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType ;}

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess=true))
	EWeaponType WeaponType;


};
