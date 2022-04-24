// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Engine/DataTable.h"
#include "WeaponTypes.h"


#include "Weapon.generated.h"



USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EAmmoType AmmoType;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 WeaponAmmo;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 MagazineSize;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class USoundCue* PickupSound;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USoundCue* EquipSound;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class UWidgetComponent* PickupWidget;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMesh* ItemMesh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString ItemName;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* InventoryIcon;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* AmmoIcon;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 MaterialIndex;
	
	

	
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

	// Public Getters and setters for private variables:
	FORCEINLINE int32 GetAmmo() const {return AmmoCount ;}
	FORCEINLINE int32 GetMagazineSize() const {return MagazineSize ;}
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType ;}
	FORCEINLINE EAmmoType GetAmmoType() const {return AmmoType ;}
	FORCEINLINE FName GetReloadMontageSection() const {return ReloadMontageSection ;}
	FORCEINLINE FName GetClipBoneName() const {return ClipBoneName ;}
	FORCEINLINE void SetMovingClip(bool Moving) {bMovingClip = Moving ;}
	
	// Called from main character class when firing weapon
	void DecreaseAmmo();

	// Called from main character class to reload the magazine 
	void ReloadAmmo(int32 AmmoAmount);
	
	bool ClipIsFull();

protected:
	
	
	void StopFalling();

	virtual void OnConstruction(const FTransform& Transform) override;
	
private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	// Ammo count for this weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess=true))
	int32 AmmoCount;

	// Maximum Ammo that weapon can hold at one time before it needs to be reloaded
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess=true))
	int32 MagazineSize;
	
	// Type of weapon this is - will tie into the HUD display
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess=true))
	EWeaponType WeaponType;

	// Type of Ammo that this weapon needs to fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess=true))
	EAmmoType AmmoType;

	// FName assigned for the reload montage we will play when reloading this wepaon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess=true))
	FName ReloadMontageSection;


	// Name of clip bone on gun's skeleton mesh, used for reloading
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess=true))
	FName ClipBoneName;

	// True while clip is being moved during reload
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess=true))
	bool bMovingClip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "My Stuff | Data Table", meta=(AllowPrivateAccess=true))
	UDataTable* WeaponDataTable;

	
	int32 PreviousMaterialIndex;
};
