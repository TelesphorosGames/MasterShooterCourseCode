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
	
	
	
	virtual void NativeInitializeAnimation() override;


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

	
	
};
