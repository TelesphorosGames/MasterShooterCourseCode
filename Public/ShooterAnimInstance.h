// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
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
};
