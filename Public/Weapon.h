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
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName ClipBoneName;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName ReloadMontageSection;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBP;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* CrossHairsMiddle;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* CrosshairsBottom;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float AutoFireRate;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USoundCue* FireSound;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName BoneToHide;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bAutomatic;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Damage;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float HeadShotDamage;

	
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
	FORCEINLINE void SetReloadMontageSection(FName Name) {ReloadMontageSection = Name ;}
	FORCEINLINE FName GetClipBoneName() const {return ClipBoneName ;}
	FORCEINLINE void SetClipBoneName(FName Name) {ClipBoneName = Name ;}
	FORCEINLINE void SetMovingClip(bool Moving) {bMovingClip = Moving ;}
	FORCEINLINE float GetAutoFireRate() const {return AutoFireRate ;}
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const {return MuzzleFlash ;}
	FORCEINLINE USoundCue* GetFireSound() const {return FireSound ;}
	FORCEINLINE bool GetAutomatic() const {return bAutomatic ;}
	FORCEINLINE float GetDamage() const {return Damage ;}
	FORCEINLINE float GetHeadShotDamage() const {return HeadShotDamage ; }


	
	
	// Called from main character class when firing weapon
	void DecreaseAmmo();

	// Called from main character class to reload the magazine 
	void ReloadAmmo(int32 AmmoAmount);
	
	bool ClipIsFull();

	void StartSlideTimer();

protected:
	
	
	void StopFalling();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	void FinishMovingSlide();

	void UpdateSlideDisplacement();
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "My Stuff| Data Tables", meta=(AllowPrivateAccess=true))
	UDataTable* WeaponDataTable;

	
	int32 PreviousMaterialIndex;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff| Data Tables", meta=(AllowPrivateAccess=true))
	UTexture2D* CrossHairsMiddle;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff| Data Tables", meta=(AllowPrivateAccess=true))
	UTexture2D* CrosshairsTop;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff| Data Tables", meta=(AllowPrivateAccess=true))
	UTexture2D* CrosshairsBottom;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff| Data Tables", meta=(AllowPrivateAccess=true))
	UTexture2D* CrosshairsLeft;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff| Data Tables", meta=(AllowPrivateAccess=true))
	UTexture2D* CrosshairsRight;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff| Data Tables", meta=(AllowPrivateAccess=true))
	float AutoFireRate;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff| Data Tables", meta=(AllowPrivateAccess=true))
	UParticleSystem* MuzzleFlash;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff| Data Tables", meta=(AllowPrivateAccess=true))
	USoundCue* FireSound;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff| Data Tables", meta=(AllowPrivateAccess=true))
	FName BoneToHide;
	/* Will drive the slide animation during pistol fire */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess=true))
	float SlideDisplacement;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess=true))
	UCurveFloat* SlideDisplacementCurve;
	FTimerHandle SlideTimer;
	float SlideDisplacementTime;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess=true))
	bool bMovingSlide;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess=true))
	float MaxSlideDisplacement;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess=true))
	float MaxRecoilRotation;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess=true))
	float RecoilRoation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess=true))
	bool bAutomatic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess=true))
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess=true))
	float HeadShotDamage;
};
