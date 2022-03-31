// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoType.h"
#include "GameFramework/Character.h"


#include "MainCharacter.generated.h"



UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_ReloadingState UMETA(DisplayName = "Reloading"),

	ECS_MAX UMETA(DisplayName = "DefaultMax"),
};

UENUM()
enum class EMovementStatus : uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_Standing UMETA(DisplayName = "Standing"),
	EMS_Sitting UMETA(DisplayName = "Sitting"),

	EMS_MAX UMETA(DisplayName = "DefaultMax"),
};


UCLASS()
class MASTERSHOOTERCOURSE_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();
	// Public getters and setters:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool GetAiming() const { return bAiming ; }
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }
	FORCEINLINE class AItem* GetItemBeingLookedAt() const { return TraceHitItemLastFrame ; }


private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Stuff | Camera", meta = (AllowPrivateAccess = "true"))
	AItem* CurrentlyLookingAtItem;
	
	//  Declares a Spring Arm Component called CameraBoom and gives it its unreal engine property values, allowing us to access it from Blueprints
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Stuff | Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// Declares a Camera Component called Follow Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My Stuff | Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	// Sound Cue component that randomizes the gunfire sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Stuff | Combat", meta =(AllowPrivateAccess= "true"))
	class USoundCue* FireSound;

	// Particles for muzzle flash effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Stuff | Combat", meta =(AllowPrivateAccess= "true"))
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Stuff | Combat", meta =(AllowPrivateAccess= "true"))
	class UAnimMontage* HipFireMontage;

	// Particles for impact point hit effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Stuff | Combat", meta =(AllowPrivateAccess= "true"))
	UParticleSystem* ImpactParticles;

		// Smoke Trail for Bullets : 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Stuff | Combat", meta =(AllowPrivateAccess= "true"))
	UParticleSystem* BeamParticles;
	
	FRotator GetLookAtRotationYaw(FVector Target) const;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	virtual void Jump() override;
	virtual void StopJumping() override;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	bool bAiming;

	float CameraDefaultFOV;

	float CameraZoomedFOV;

	float CameraCurrentFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Combat", meta=(AllowPrivateAccess=true))
	float ZoomInterpSpeed;


	//* These variables determine how much the crosshairs will spread on screen :

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	float CrosshairSpreadMultiplier;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	float CrosshairVelocityFactor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	float CrosshairInAirFactor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))	
	float CrosshairAimFactor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	float CrosshairShootingFactor;

	bool bFiringBullet;

	// Timer for determining how long to spread the crosshairs apart on the screen when firing a shot
	FTimerHandle CrosshairShootTimer;
	float ShootTimeDuration;

	bool bShouldTraceForItems;

	int8 OverlappedItemCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	AItem* TraceHitItemLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	class AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	TSubclassOf<AWeapon> DefaultWeaponClass;


	/*The item being hit by our line trace in TraceForItems() at any given time
	 * COULD BE NULL */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	AItem* TraceHitItem;


	/* Distance outward from the camera in the forward direction used for determining where items
	* are shown to the player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	float CameraInterpDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	float CameraInterpElevation;

	// This TMAP will keep track of ammo types and their associated integer amount
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Combat", meta = (AllowPrivateAccess=true))
	TMap<EAmmoType, int32> AmmoMap;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "My Stuff | Combat", meta = (AllowPrivateAccess=true))
	int32 StartingPistolAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "My Stuff | Combat", meta = (AllowPrivateAccess=true))
	int32 StartingAssaultRifleAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Combat", meta = (AllowPrivateAccess=true))
	ECombatState CombatState;

	// Animation montage for reloading the different weapons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Stuff | Combat", meta =(AllowPrivateAccess= "true"))
	UAnimMontage* ReloadMontage;

	//Used to track the gun's clip location during reload so we can attach it to our hand
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Combat", meta = (AllowPrivateAccess=true))
	FTransform ClipTransform;

	//used to track the hand's location to attach the clip to during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Combat", meta = (AllowPrivateAccess=true))
	USceneComponent* HandClipLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Combat", meta = (AllowPrivateAccess=true))
	EMovementStatus MovementStatus;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void PlayGunFireSound();
	void FireOneBullet();
	void PlayRecoilAnimation();

	// Called When Fire Button is pressed
	void FireWeapon();

	bool GetBeamEndLocation(const FVector &MuzzleSocketLocation, FVector &OutBeamLocation);

	void AimingButtonPressed();
	void AimingButtonReleased();

	void SetZoomInterp(float DeltaTime);

	void SetLookRates();

	void CalculateCrosshairSpread(float DeltaTime);

	void StartCrossHairBulletFire();
	
	UFUNCTION()
	void FinishCrosshairBulletFire();
	void TraceForItems();

	AWeapon* SpawnDefaultWeapon();

	void EquipWeapon(AWeapon* WeaponToEquip);


	// Used to let a weapon in our hands drop to the ground
	void DropWeapon();

	void TestButtonPressed();
	void TestButtonReleased();

	
	void SwapWeapon(AWeapon* WeaponToSwap);

	
	void InitializeAmmoMap();

	
	bool WeaponHasAmmo();

	// Bound to Reload key, handles all functions attached to input
	void ReloadButtonPressed();

	// Handles reloading animation and refills magazine for gun
	void ReloadWeapon();

	// Returns true if our character has the equipped weapon's ammo type - used for reloading
	bool CarryingAmmo();
	
	// Will be called when the anim notify is reached to signal the end of the reload animation
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	// Handle Animations for moving the clip during reloading
	UFUNCTION(BlueprintCallable)
	void GrabClip();
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	void ToggleSit();

	
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Camera")
	float BaseTurnRate =15.f ;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Camera")
	float BaseLookUpRate = 15.f ;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff")
	float HipTurnRate = 35.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff")
	float HipLookUpRate = 35.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff")
	float AimingTurnRate = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff")
	float AimingLookUpRate = 10.f;

	bool bFireButtonPressed;

	bool bShouldFire;

	FTimerHandle AutoFireTimer;

	float AutoFireRate = 0.15f;

	void FireButtonPressed();

	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	// Line Trace For Items Under Crosshairs
	bool TraceUnderCrosshairs(FHitResult &OutHit, FVector &OutHitBeamEnd);

	void IncrementOverlappedItemCount(int8 Amount);


	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FVector GetCameraInterpLocation();
	
	void GetPickupItem(AItem* Item);

	
};
