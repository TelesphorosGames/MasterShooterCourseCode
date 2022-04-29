// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.h"
#include "Animation/AnimInstance.h"



#include "ShooterAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName="Aiming"),
	EOS_HipFire UMETA(DisplayName="HipFire"),
	EOS_Reloading UMETA(DisplayName="Reloading"),
	EOS_InAir UMETA(DisplayName="InAir"),
	EOS_RunningAiming UMETA(DisplayName="RunningAiming"),
	
	EOS_MAX UMETA(DisplayName="DefaultMax")
};
UCLASS()
class MASTERSHOOTERCOURSE_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);
	
	UShooterAnimInstance();
	
	virtual void NativeInitializeAnimation() override;
	void SetRecoilAndReloadWeights();

	FVector2D BulletTarget2d;

protected:

	// Will handle the turning in place variables
	void TurnInPlace();
	
	UFUNCTION(BlueprintCallable)
	void AdjustAimOffset(float DeltaTime, float &OutYaw, float &OutPitch, float InYaw = 0.f, float InPitch =0.f);

	//Handles Calculations for leaning while running
	void Lean(float DeltaTime);

	float YawExcess = 0.f;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	class AMainCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	float Speed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	bool bIsAccelerating;

	/**
	 * @brief This value is required in order for our animation to understand which way it should be strafing,
	 * according to calculations done between the movement speed and movement direction. It allows our blendspaces
	 * to work properly.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	float MovementOffsetYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	float LastMovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	bool bAiming;

	// Yaw of the Character This frame - used for turn in place anims
	float CharacterYaw;

	
	//Yaw of the character in the previous frame - used for turn in place anims
	float CharacterYawLastFrame;

	// Delta yaw value used for leaning while running animations
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,  Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	float YawLeanDelta;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	float RootYawOffset;

	//Rotation Curve Value used for turn in place anims
	float RotationCurve;

	// Rotation Curve value held from last frame used for turn in place anims
	float RotationCurveLastFrame;

	// Our character's pitch value used for aim offset
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	float Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	float UpdatedPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	float UpdatedYaw;

	// Used to re-center our character when reloading to prevent an animation offset for the gun's clip
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	bool bReloading;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	bool bEquipping;

	// Will be used to determine which aim offset blendspace to use in our animation blueprint 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	EOffsetState OffsetState;

	FRotator LeanCharacterRotation;

	FRotator LeanCharacterRotationLastFrame;

	// Powers the crouching anims, true when our character is crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	bool bCrouchingForAnims;

	// Changes how much recoil our animation will have to help clean up our turn in place and crouch animations
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Animation", meta = (AllowPrivateAccess=true))
	float RecoilWeight;

	// Changes how much weight our reload animation will hold in our animation BP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Animation", meta = (AllowPrivateAccess=true))
	float ReloadWeight;

	// True when we are turning in place, used with RecoilWeight and ReloadWeight
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category= "My Stuff | Movement", meta = (AllowPrivateAccess=true))
	bool bTurningInPlace;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,  Category= "My Stuff | Combat", meta = (AllowPrivateAccess = true))
	EWeaponType EquippedWeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,  Category= "My Stuff | Combat", meta = (AllowPrivateAccess = true))
	bool bShouldUseFabrik; 
	
};
