// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"

#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if(ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AMainCharacter>(TryGetPawnOwner());
	}
	if(ShooterCharacter)
	{
		//Get later Speed of Character from Velocity - speed from falling is not taken into account

		FVector Velocity = ShooterCharacter->GetVelocity();
		Velocity.Z = 0.f;
		Speed = Velocity.Size();

		// Char in air? 

		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// Is Char moving at all?

		if(ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size()> 0)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		const FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		if(ShooterCharacter->GetVelocity().Size() > 0.f)
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}

		bAiming = ShooterCharacter->GetAiming();

		// *** USE TO CREATE FSTRINGS WITH Printf AND GEngine->AddOnscreenDebugMessage ***//
		//
		// FString MovementOffsetMessage = FString::Printf(TEXT("%f"), MovementOffsetYaw);
		//
		// if(GEngine)
		// {
		// 	
		// 	// FString RotationMessage = FString::Printf(TEXT("Base Aim Roation : %f"), AimRotation.Yaw);
		// 	// FString MovementRotationMessage = FString::Printf(TEXT("%f"), MovementRotation.Yaw);
		// 	GEngine->AddOnScreenDebugMessage(1, 0, FColor::White, MovementOffsetMessage);
		// }
		
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	// Super::NativeInitializeAnimation();
	ShooterCharacter = Cast<AMainCharacter>(TryGetPawnOwner());
	
}
