// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UCLASS()
class MASTERSHOOTERCOURSE_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetAiming() const { return bAiming ; }

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	FORCEINLINE class AItem* GetItemBeingLookedAt() const { return CurrentlyLookingAtItem ; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;
	
	bool bItemHudCurrentlyDisplayed;
	
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
	FTimerHandle CrosshairShootTimer;
	float ShootTimeDuration;

	bool bShouldTraceForItems;

	int8 OverlappedItemCount;
	


	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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
	float HipTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff")
	float HipLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff")
	float AimingTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff")
	float AimingLookUpRate;

	bool bFireButtonPressed;

	bool bShouldFire;

	FTimerHandle AutoFireTimer;

	float AutoFireRate;

	void FireButtonPressed();

	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	// Line Trace For Items Under Crosshairs
	bool TraceUnderCrosshairs(FHitResult &OutHit, FVector &OutHitBeamEnd);

	void IncrementOverlappedItemCount(int8 Amount);

	
	
	

	
};
