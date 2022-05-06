// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"


#include "MainCharacter.h"
#include "SAdvancedRotationInputBox.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
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
	YawLeanDelta(0.f),
	RootYawOffset(0.f),
	Pitch(0.f),
	bReloading(false),
	OffsetState(EOffsetState::EOS_HipFire),
	LeanCharacterRotation(FRotator(0.f)),
	LeanCharacterRotationLastFrame(FRotator(0.f)),
	RecoilWeight(1.f),
	bTurningInPlace(false),
	ReloadWeight(.85f),
	bShouldUseFabrik(true),
	EquippedWeaponType(EWeaponType::EWT_MAX)

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
		bCrouchingForAnims = ShooterCharacter->GetCrouching();

		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_ReloadingState;
		bEquipping = ShooterCharacter->GetCombatState() == ECombatState::ECS_PickingUpWeapon;

		bShouldUseFabrik = ShooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied || ShooterCharacter->
			GetCombatState() == ECombatState::ECS_FireTimerInProgress || ShooterCharacter->GetCombatState() == ECombatState::ECS_PickingUpWeapon;

		//Get lateral Speed of Character from Velocity - speed from falling is not taken into account
		FVector Velocity = ShooterCharacter->GetVelocity();
		Velocity.Z = 0.f;
		Speed = Velocity.Size();

		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

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

		if (bReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if (bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (ShooterCharacter->GetAiming())
		{
			if (Speed > 0)
			{
				OffsetState = EOffsetState::EOS_RunningAiming;
			}
			else
			{
				OffsetState = EOffsetState::EOS_Aiming;
			}
		}
		else
		{
			OffsetState = EOffsetState::EOS_HipFire;
		}
	}

	if (ShooterCharacter && ShooterCharacter->GetEquippedWeapon())
	{
		EquippedWeaponType = ShooterCharacter->GetEquippedWeapon()->GetWeaponType();
	}
	AdjustAimOffset(LastMovementOffsetYaw, Pitch, UpdatedYaw, UpdatedPitch);
	TurnInPlace();
	Lean(DeltaTime);
	SetRecoilAndReloadWeights();
}


void UShooterAnimInstance::NativeInitializeAnimation()
{
	// Super::NativeInitializeAnimation();
	ShooterCharacter = Cast<AMainCharacter>(TryGetPawnOwner());
	UpdatedPitch = Pitch;
	UpdatedYaw = RootYawOffset;
}

void UShooterAnimInstance::SetRecoilAndReloadWeights()
{
	if (bTurningInPlace) /* TURNING IN PLACE  */
	{
		if (bReloading || bEquipping) // Turning in place, while reloading or equipping
		{
			RecoilWeight = 1.f; // Full reload animation, Full Recoil Animation
			if (bCrouchingForAnims)
			{
				ReloadWeight = 0.f; // NO AIM ANIM, ALL RELOAD ANIM
			}
		}
		else
		{
			RecoilWeight = 0.f; // No reload animation, no recoil animation
		}
	}
	else /* NOT turning in place */
	{
		if (bCrouchingForAnims) // Crouching not turning
		{
			if (bReloading || bEquipping) // Crouching, while reloading or equipping
			{
				RecoilWeight = 1.f; // Full Reload animation
				ReloadWeight = 0.f;
			}
			else if (bAiming)
			{
				RecoilWeight = .5f; // Half recoil
				ReloadWeight = .8f; // Mostly aim anim with Recoil mixed in
			}
			else // Crouching hip fire stance
			{
				ReloadWeight = .8f;
				RecoilWeight = 0.2f; // Very Little Recoil
			}
		}
		else // standing upright
		{
			if (bAiming) // Standing still, upright, Aiming 
			{
				if (bReloading) // and reloading
				{
					RecoilWeight = .95f; // full reload animation
					ReloadWeight = 0.f; // NO AIM ANIM, ALL RELOAD ANIM
				}
				else // just aiming
				{
					RecoilWeight = .5f; // Half recoil
					ReloadWeight = .8f; // Mostly aim anim with Recoil mixed in
				}
			}
			else if (bReloading || bEquipping) // standing upright, not aiming, and reloading or Equipping
			{
				RecoilWeight = .9f; // Full Recoil Animation
				ReloadWeight = 0.f; // NO AIM ANIM, ALL RELOAD ANIM
			}
			else // Basic hipfire stance
			{
				RecoilWeight = .5f; // Half recoil
				ReloadWeight = .8f; // Mostly aim anim with Recoil mixed in
			}
		}
	}
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	if (Speed > 0.001f || (bIsInAir))
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
		const float YawTIPDelta = {(CharacterYaw - CharacterYawLastFrame)};

		// Clamps Root Yaw Offset to -180, 180
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - YawTIPDelta);
		
		// Metadata attached to our curve for turn in place animations
		const float Turning{GetCurveValue(TEXT("Turning"))};
		if (Turning > 0) /* TURNING IN PLACE  */
		{
			bTurningInPlace = true;
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			// Giving us the very small amount that we are turning each frame, using our rotation curve data
			const float DeltaRotation{RotationCurve - RotationCurveLastFrame};

			/* If the RootYawOffset is positive, ( Greater than 0 ) we are turning left.
			 * If it is negative, we are turning right.
			*/
			// Determines if we are turning right or left
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			// Getting the Absolute value of Root Yaw Offset and compensating by finding the excess amount ( after 90 degreees ) and subtracting it / adding it depending on direction
			const float ABSRootYawOffset{FMath::Abs(RootYawOffset)};
			if (ABSRootYawOffset > 90.f)
			{
				YawExcess = {ABSRootYawOffset - 90.f};
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
				// RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		else /* NOT turning in place */
		{
			bTurningInPlace = false;
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

void UShooterAnimInstance::AdjustAimOffset(float& OutYaw, float& OutPitch, const float InYaw,const float InPitch)
{
	if (!ShooterCharacter || bReloading || bEquipping) return;

	if (ShooterCharacter->GetEquippedWeapon())
	{
		FVector2D ViewportSize;

		FVector BulletTarget;
		ShooterCharacter->GetBeamEndLocation(
			ShooterCharacter->GetEquippedWeapon()->GetItemMesh()->GetSocketLocation(FName("BarrelSocket")),
			BulletTarget);

		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), BulletTarget, BulletTarget2d);
		
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);

			float YScreenSpacePercent = UKismetMathLibrary::NormalizeToRange(BulletTarget2d.Y, 0, ViewportSize.Y);
			float XScreenSpacePercent = UKismetMathLibrary::NormalizeToRange(BulletTarget2d.X, 0, ViewportSize.X);
			
			if (!(YScreenSpacePercent > .49f && YScreenSpacePercent < .51f))
			{
				if (YScreenSpacePercent < .49f)
				{
					UpdatedPitch -= (.5f - YScreenSpacePercent) * 25;
				}
				if (YScreenSpacePercent > .51f)
				{
					UpdatedPitch += FMath::Abs(.5f - YScreenSpacePercent) * 25;
				}
			}

			if (!(XScreenSpacePercent > .49f && XScreenSpacePercent < .51f))
			{
				if (XScreenSpacePercent < .49f)
				{
					UpdatedYaw -= (.5f - XScreenSpacePercent) * 25;
				}
				if (XScreenSpacePercent > .51f)
				{
					UpdatedYaw += FMath::Abs(.5f - XScreenSpacePercent) * 25;
				}
			}
			// UE_LOG(LogTemp, Warning, TEXT("Yaw : %f"), CharacterYaw);
			// UE_LOG(LogTemp, Warning, TEXT("UpdatedYaw : %f"), UpdatedYaw);
			
			float CurrentPitchTarget = UKismetMathLibrary::Lerp(InYaw, UpdatedYaw, 1);
			float CurrentYawTarget = UKismetMathLibrary::Lerp(InPitch, UpdatedPitch, 1);
			
			if (UpdatedPitch > 180 || UpdatedPitch < -180) UpdatedPitch = InPitch;
			if (UpdatedYaw > 200 || UpdatedYaw < -200) UpdatedYaw = InYaw;

			
			OutPitch = CurrentPitchTarget;
			OutYaw = CurrentYawTarget;
		}
	}
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr) return;
	LeanCharacterRotationLastFrame = LeanCharacterRotation;
	LeanCharacterRotation = ShooterCharacter->GetActorRotation();

	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(
	LeanCharacterRotation, LeanCharacterRotationLastFrame);

	// This gives us a measure of how quickly we are turning
	const float TargetYaw = DeltaRotation.Yaw / DeltaTime;
	// Interolates yaw delta towards its target
	const float TargetYawInterp = FMath::FInterpTo(YawLeanDelta, TargetYaw, DeltaTime, 6.f);

	// Clamps the yaw delta to a value between -90 degrees and 90 degrees 
	YawLeanDelta = FMath::Clamp(TargetYawInterp, -90.f, 90.f);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(3, -1, FColor::Emerald,
		                                 FString::Printf(TEXT("YawLeanDelta: %f"), YawLeanDelta));
}
