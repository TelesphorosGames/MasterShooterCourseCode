// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Item.h"
#include "Components/WidgetComponent.h"


// Sets default values
AMainCharacter::AMainCharacter() :
	bItemHudCurrentlyDisplayed(false),
	CurrentlyLookingAtItem(nullptr),
	bAiming(false),
	CameraDefaultFOV(0.f),

	// Camera Field of View Defaults 
	CameraZoomedFOV(40.f),
	ZoomInterpSpeed(20.f),
	CrosshairSpreadMultiplier(0.f),
	//Crosshair Spread Factors
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	bFiringBullet(false),
	ShootTimeDuration(0.1f),
	bShouldTraceForItems(false),
	// Base Turn Rates
	BaseTurnRate(15.f),
	BaseLookUpRate(15.f),
	// Turn Rates For Aiming / Not Aiming 
	HipTurnRate(35.f),
	HipLookUpRate(35.f),
	AimingTurnRate(10.f),
	AimingLookUpRate(10.f),
	//Automatic Fire Rate in Seconds
	bShouldFire(true),
	AutoFireRate(0.15f)

{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Creates and initializes the Camera Springarm and Third Person Camera attached to it:
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = {0.f, 70.f, 70.f};
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;


	// Don't rotate the pawn with controller -
	// Only rotate the camera with the controller.
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Other values for character movement - movement orientation, Rotation rate, jump velocity, and air control
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator{0.f, 540.f, 0.f};
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = .2f;
}

float AMainCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target) const
{
	// Using UKismetMathLibrary's 
	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	const FRotator LookAtRotationYaw = {0.f, LookAtRotation.Yaw, 0.f};
	return LookAtRotationYaw;
}

void AMainCharacter::MoveForward(float Value)
{
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation = {0, Rotation.Yaw, 0};
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation = {0, Rotation.Yaw, 0};
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::Jump()
{
	Super::Jump();
}

void AMainCharacter::StopJumping()
{
	Super::StopJumping();
}


void AMainCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}


// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera)
	{
		CameraDefaultFOV = FollowCamera->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AMainCharacter::FireWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon Fired!"));
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FireSound);
	}
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		const bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
		if (bBeamEnd)
		{
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);
			}
			if (BeamParticles)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(), BeamParticles, SocketTransform);
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}

		//**
		// FHitResult FireHitResult;
		// const FVector Start = SocketTransform.GetLocation();
		// const FQuat Rotation = SocketTransform.GetRotation();
		// const FVector RoationAxis = Rotation.GetAxisX();
		// const FVector End = Start + RoationAxis * 50'000;
		//
		// FVector BeamEndPoint = End;
		//
		// GetWorld()->LineTraceSingleByChannel(FireHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		// if(FireHitResult.bBlockingHit)
		// {
		// 	BeamEndPoint = FireHitResult.Location;
		// 	DrawDebugLine(GetWorld(), Start, End, FColor::Emerald, false, 3.f);
		// 	DrawDebugPoint(GetWorld(), FireHitResult.Location, 10, FColor::Red, false, 3.5f);
		//
		// 	if(ImpactParticles)
		// 	{
		// 		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHitResult.Location);
		// 	}
		// }
		// if(BeamParticles)
		// {
		// 	UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);
		// 	Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
		// }

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && HipFireMontage)
		{
			AnimInstance->Montage_Play(HipFireMontage);
			AnimInstance->Montage_JumpToSection(FName("StartFire"));
		}
	}

	StartCrossHairBulletFire();
}

bool AMainCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	FHitResult CrosshairHitResult;

	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else
	{
	}
	FHitResult WeaponTraceHit;


	const FVector WeaponTraceStart = MuzzleSocketLocation;
	const FVector StartToEnd = OutBeamLocation - MuzzleSocketLocation;
	const FVector WeaponTraceEnd = MuzzleSocketLocation + StartToEnd * 1.25f;

	GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECC_Visibility);
	if (WeaponTraceHit.bBlockingHit)
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}

	return false;
}

void AMainCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AMainCharacter::AimingButtonReleased()
{
	bAiming = false;
}

void AMainCharacter::SetZoomInterp(float DeltaTime)
{
	if (bAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	FollowCamera->SetFieldOfView(CameraCurrentFOV);
}

void AMainCharacter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AMainCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	const FVector2D WalkSpeedRange = {0.f, 600.f};
	const FVector2D VelocityMultiplierRange = {0.f, 1.f};
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange,
	                                                            Velocity.Size());

	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 1.25f, DeltaTime, 10.0f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 10.25f);
	}

	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 1.f, DeltaTime, 10.5f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 10.5f);
	}

	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, .5f, DeltaTime, 10.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 10.f);
	}

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - (CrosshairAimFactor / 2) +
		CrosshairShootingFactor;
}

void AMainCharacter::StartCrossHairBulletFire()
{
	bFiringBullet = true;
	GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AMainCharacter::FinishCrosshairBulletFire,
	                                ShootTimeDuration);
}

void AMainCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

void AMainCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitResultHolder;
		TraceUnderCrosshairs(ItemTraceResult, HitResultHolder);
		if (ItemTraceResult.bBlockingHit)
		{
			AItem* HitItem = Cast<AItem>(ItemTraceResult.GetActor());
			if (HitItem && HitItem->GetPickupWidget())
			{
				if (CurrentlyLookingAtItem != HitItem)
                    {
                        CurrentlyLookingAtItem = HitItem;
                    }

				if (CurrentlyLookingAtItem->bItemInRangeForHUD() && !bItemHudCurrentlyDisplayed)
				{
					
					CurrentlyLookingAtItem->GetPickupWidget()->SetVisibility(true);
					bItemHudCurrentlyDisplayed = true;
				}
				
			}

			else if (CurrentlyLookingAtItem != nullptr && CurrentlyLookingAtItem != HitItem)
			{
				CurrentlyLookingAtItem->GetPickupWidget()->SetVisibility(false);
				bItemHudCurrentlyDisplayed = false;
				CurrentlyLookingAtItem = nullptr;
			}
		}
	}
	else
	{
		CurrentlyLookingAtItem = nullptr;
	}
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetZoomInterp(DeltaTime);
	SetLookRates();
	CalculateCrosshairSpread(DeltaTime);

	TraceForItems();
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainCharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AMainCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AMainCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMainCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMainCharacter::AimingButtonReleased);
}

void AMainCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	StartFireTimer();
}

void AMainCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AMainCharacter::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AMainCharacter::AutoFireReset, AutoFireRate);
	}
}

void AMainCharacter::AutoFireReset()
{
	bShouldFire = true;
	if (bFireButtonPressed)
	{
		StartFireTimer();
	}
}

bool AMainCharacter::TraceUnderCrosshairs(FHitResult& OutHit, FVector& OutHitBeamEnd)
{
	//Get Viewport Size:
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation = {(ViewportSize.X / 2.f), (ViewportSize.Y / 2.f)};


	FVector CrossHairWorldPosition;
	FVector CrossHairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
	                                                               CrosshairLocation, CrossHairWorldPosition,
	                                                               CrossHairWorldDirection);

	if (bScreenToWorld)
	{
		//Trace from Crosshair World Location Outward - Ray cast to see if any item collisions occur

		const FVector Start = {CrossHairWorldPosition};
		const FVector End = {Start + CrossHairWorldDirection * 50'000};
		OutHitBeamEnd = End;

		GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility);
		if (OutHit.bBlockingHit)
		{
			OutHitBeamEnd = OutHit.Location;
			return true;
		}
	}

	return false;
}