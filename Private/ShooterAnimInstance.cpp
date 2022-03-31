// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"

#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


UShooterAnimInstance::UShooterAnimInstance() :
	Speed(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	MovementOffsetYaw(0.f),
	LastMovementOffsetYaw(0.f),
	bAiming(false),
	CharacterYaw(0.f),
	CharacterYawLastFrame(0.f),
	RootYawOffset(0.f)
{
}


void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AMainCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter)
	{
		//Get later Speed of Character from Velocity - speed from falling is not taken into account

		FVector Velocity = ShooterCharacter->GetVelocity();
		Velocity.Z = 0.f;
		Speed = Velocity.Size();

		// Char in air? 

		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// Is Char moving at all?

		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
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

		if (ShooterCharacter->GetVelocity().Size() > 0.f)
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

	TurnInPlace();
}


void UShooterAnimInstance::NativeInitializeAnimation()
{
	// Super::NativeInitializeAnimation();
	ShooterCharacter = Cast<AMainCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;
	if (Speed > 0)
	{
		RootYawOffset = 0.f;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		CharacterYawLastFrame = CharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		CharacterYawLastFrame = CharacterYaw;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		// Difference between yaw last frame and this frame
		const float YawDelta{CharacterYaw - CharacterYawLastFrame};

		// Clamps Root Yaw Offset to -180, 180
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawDelta);
		// Metadata attached to our curve for turn in place animations

		const float Turning{GetCurveValue(TEXT("Turning"))};
		if (Turning > 0)
		{
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			// Giving us the very small amount that we are turning each frame, using our rotation curve data
			const float DeltaRotation{RotationCurve - RotationCurveLastFrame};

			/* If the RootYawOffset is positive, ( Greater than 0 ) we are turning left. If it is negative, we are turning right.
			*/
			// Determines if we are turning right or left
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			// Getting the Absolute value of Root Yaw Offset and compensating by finding the excess amount ( after 90 degreees ) and subtracting it / adding it depending on direction

			const float ABSRootYawOffset{FMath::Abs(RootYawOffset)};
			if (ABSRootYawOffset > 90.f)
			{
				const float YawExcess{ABSRootYawOffset - 90.f};
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, -1, FColor::Blue,
			                                 FString::Printf(TEXT("Character Yaw: %f"), CharacterYaw));
			GEngine->AddOnScreenDebugMessage(2, -1, FColor::Red,
			                                 FString::Printf(TEXT("Root Yaw Offset: %f"), RootYawOffset));
		}
	}
}

// Test Comment

//Approved comments

