// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoType.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"


#include "MainCharacter.generated.h"



UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_ReloadingState UMETA(DisplayName = "Reloading"),
	ECS_PickingUpAmmo UMETA(DisplayName = "PickingUpAmmo"),
	ECS_PickingUpWeapon UMETA(DisplayName ="PickingUpWeapon"),

	ECS_MAX UMETA(DisplayName = "DefaultMax"),
};

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_Standing UMETA(DisplayName = "Standing"),
	EMS_Sitting UMETA(DisplayName = "Sitting"),
	EMS_Crouching UMETA(DisplayName= "Crouching"),

	EMS_MAX UMETA(DisplayName = "DefaultMax"),
};

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()
	// Scene component used to decide which location to interpolate the pickup to
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;
	// Number of items interping to this scene component's location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);

UCLASS()
class MASTERSHOOTERCOURSE_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent*  PlayerInputComponent) override;
	void InitializeWeaponSockets();


	bool GetBeamEndLocation(const FVector &MuzzleSocketLocation, FVector &OutBeamLocation);
protected:
	
// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool GetAiming() const { return bAiming ; }
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }
	FORCEINLINE class AItem* GetItemBeingLookedAt() const { return TraceHitItemLastFrame; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetCrouching() const {return bCrouching; }
	FORCEINLINE class AWeapon* GetEquippedWeapon() const { return EquippedWeapon ; } 

	// Handles finding where our picked up items should interpolate to on our screen
	
	FVector BeamEndPublic;

	FVector CrosshairToWorld;

	FVector CrossHairPublicHit = {0,0,0};
	
private:

	
	
	
	
	bool bNothingHit;

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

	void AdjustCameraLengthUp();
	void AdjustCameraLengthDown();
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Stuff | Combat", meta =(AllowPrivateAccess= "true"))
	UAnimMontage* EquipMontage;
	
	//Used to track the gun's clip location during reload so we can attach it to our hand
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Combat", meta = (AllowPrivateAccess=true))
	FTransform ClipTransform;

	//used to track the hand's location to attach the clip to during reloading
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Combat", meta = (AllowPrivateAccess=true))
	USceneComponent* HandClipLocation;

	// Our movement status, used for animation checks
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Movement", meta = (AllowPrivateAccess=true))
	EMovementStatus MovementStatus;

	// True when crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Movement", meta = (AllowPrivateAccess=true))
	bool bCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Movement", meta = (AllowPrivateAccess=true))
	float CrouchMovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Movement", meta = (AllowPrivateAccess=true))
	float BaseMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Stuff | Combat", meta =(AllowPrivateAccess= "true"))
	float CurrentCapsuleHalfHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Stuff | Combat", meta =(AllowPrivateAccess= "true"))
	float StandingCapsuleHalfHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Stuff | Combat", meta =(AllowPrivateAccess= "true"))
	float CrouchingCapsuleHalfHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	USceneComponent* WeaponInterpComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	USceneComponent* InterpComp1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	USceneComponent* InterpComp2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	USceneComponent* InterpComp3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	USceneComponent* InterpComp4;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	USceneComponent* InterpComp5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	USceneComponent* InterpComp6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Combat", meta = (AllowPrivateAccess = true))
	TArray<FInterpLocation> InterpLocations;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Inventory", meta=(AllowPrivateAccess = true))
	TArray<AItem*> Inventory;

	const int32 InventorySize = 6;

	UPROPERTY(BlueprintAssignable, Category = "My Stuff | Delegates", meta=(AllowPrivateAccess = true))
	FEquipItemDelegate EquipItemDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "My Stuff | Delegates", meta=(AllowPrivateAccess = true))
	FHighlightIconDelegate HighlightIconDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Inventory", meta =(AllowPrivateAccess = true))
	int32 HighlightedSlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Inventory", meta =(AllowPrivateAccess = true))
	AWeapon* WeaponInExchange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Inventory", meta =(AllowPrivateAccess = true))
	AWeapon* NewWeaponInExchange;

protected:
	
	void PlayGunFireSound();
	void FireOneBullet();
	void PlayRecoilAnimation();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToSocket(AWeapon* Weapon);
	
	// Called When Fire Button is pressed
	void FireWeapon();

	

	void AimingButtonPressed();
	void AimingButtonReleased();

	bool bAimingButtonPressed;
	

	void Aim();
	void StopAiming();

	void SetZoomInterp(float DeltaTime);

	void InitializeInterpLocations();

	

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

	void GKeyPressed();
	void OneKeyPressed();
	void TwoKeyPressed();
	void ThreeKeyPressed();
	void FourKeyPressed();
	void FiveKeyPressed();
	
	void SwapWeapon(AWeapon* WeaponToSwap);
	void PickupAmmo(class AAmmo* Ammo);
	
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

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void FinishDisarming();
	
	// Handle Animations for moving the clip during reloading
	UFUNCTION(BlueprintCallable)
	void GrabClip();
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

	void ToggleSit();

	void CrouchButtonPressed();
	
	void InterpCapsuleHalfHeight();

	void ExchangeInventoryItems(int32 CurrentIndex, int32 NewItemIndex);

	int32 GetEmptyInventorySlot();

	
	
public:	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Camera")
	float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Camera")
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff")
	float HipTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff")
	float HipLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff")
	float AimingTurnRate ;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff")
	float AimingLookUpRate;

	bool bFireButtonPressed;

	bool bShouldFire;

	FTimerHandle AutoFireTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff")
	float AutoFireRate = 0.1f;

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

	// Returns index in InterpLocations array with lowest item count
	int32 GetInterpLocationIndex();
	FInterpLocation GetInterpLocation(int32 Index);
	void IncrementInterpLocItemCount(int32 Index, int32 Amount);
	void HighlightInventorySlot();
	void UnHighlightInventorySlot();
};
