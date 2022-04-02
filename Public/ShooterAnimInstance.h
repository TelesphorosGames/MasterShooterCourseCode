// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"



#include "ShooterAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName="Aiming"),
	EOS_HipFire UMETA(DisplayName="HipFire"),
	EOS_Reloading UMETA(DisplayName="Reloading"),
	EOS_InAir UMETA(DisplayName="InAir"),
	
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

protected:

	// Will handle the turning in place variables
	void TurnInPlace();

	//Handles Calculations for leaning while running
	void Lean(float DeltaTime);


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

	// Used to re-center our character when reloading to prevent an animation offset for the gun's clip
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	bool bReloading;

	// Will be used to determine which aim offset blendspace to use in our animation blueprint 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	EOffsetState OffsetState;

	FRotator LeanCharacterRotation;

	FRotator LeanCharacterRotationLastFrame;

	// Powers the crouching anims, true when our character is crouching
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "My Stuff | Movement", meta = (AllowPrivateAccess = true))
	bool bCrouchingForAnims;
	
	
};
