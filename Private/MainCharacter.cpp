// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"

#include "Ammo.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Item.h"
#include "Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"


// Sets default values
AMainCharacter::AMainCharacter() :

	CurrentlyLookingAtItem(nullptr),
	bAiming(false),
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(40.f),
	ZoomInterpSpeed(20.f),
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimFactor(0.f),
	CrosshairShootingFactor(0.f),
	bFiringBullet(false),
	ShootTimeDuration(0.1f),
	bShouldTraceForItems(false),
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),
	StartingPistolAmmo(85),
	StartingAssaultRifleAmmo(120),
	CombatState(ECombatState::ECS_Unoccupied),
	MovementStatus(EMovementStatus::EMS_Standing),
	bShouldFire(true),
	BaseMovementSpeed(675.f),
	CrouchMovementSpeed(225.f),
	CurrentCapsuleHalfHeight(88.f),
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	BaseTurnRate(105.f),
	BaseLookUpRate(95.f),
	HipTurnRate(105.f),
	HipLookUpRate(95.f),
	AimingTurnRate(25.f),
	AimingLookUpRate(25.f),
	bNothingHit(false)
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

	// Creates the Scene Component used to trach the hand and gun clip movement during reload
	HandClipLocation = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComponent"));

	// Create scene components that are used for item locations on screen during pickup
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponInterpolationComponent"));
	WeaponInterpComp->SetupAttachment(GetFollowCamera());
	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemInterpolationComponent1"));
	InterpComp1->SetupAttachment(GetFollowCamera());
	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemInterpolationComponent2"));
	InterpComp2->SetupAttachment(GetFollowCamera());
	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemInterpolationComponent3"));
	InterpComp3->SetupAttachment(GetFollowCamera());
	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemInterpolationComponent4"));
	InterpComp4->SetupAttachment(GetFollowCamera());
	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemInterpolationComponent5"));
	InterpComp5->SetupAttachment(GetFollowCamera());
	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemInterpolationComponent6"));
	InterpComp6->SetupAttachment(GetFollowCamera());
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetZoomInterp(DeltaTime);
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

	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainCharacter::StopJumping);

	PlayerInputComponent->BindAction("AdjustCameraLengthUp", IE_Pressed, this, &AMainCharacter::AdjustCameraLengthUp);
	PlayerInputComponent->BindAction("AdjustCameraLengthDown", IE_Pressed, this,
									 &AMainCharacter::AdjustCameraLengthDown);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AMainCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AMainCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMainCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMainCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("Test", IE_Pressed, this, &AMainCharacter::TestButtonPressed);
	PlayerInputComponent->BindAction("Test", IE_Released, this, &AMainCharacter::TestButtonReleased);

	PlayerInputComponent->BindAction("Sit", IE_Pressed, this, &AMainCharacter::ToggleSit);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::CrouchButtonPressed);

	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &AMainCharacter::ReloadButtonPressed);
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

	//Spawns the default weapon and equips it 
	EquipWeapon(SpawnDefaultWeapon());
	InitializeAmmoMap();
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	InitializeInterpLocations();
}

float AMainCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

FVector AMainCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation = FollowCamera->GetComponentLocation();

	const FVector CameraForwardDirection = FollowCamera->GetForwardVector();

	return CameraWorldLocation + (CameraForwardDirection * CameraInterpDistance) + (FVector
		{0, 0, CameraInterpElevation});
}

void AMainCharacter::GetPickupItem(AItem* Item)
{
	if (Item->GetEquipSound())
	{
		UGameplayStatics::PlaySound2D(this, Item->GetEquipSound());
	}

	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		SwapWeapon(Weapon);
	}

	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo)
	{
		PickupAmmo(Ammo);
	}
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
	float ClampedRate = FMath::Clamp(Rate, -1, 1);
	if (!bAiming)
	{
		AddControllerYawInput(ClampedRate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
	else
	{
		AddControllerYawInput(ClampedRate * AimingTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	float ClampedRate = FMath::Clamp(Rate, -1, 1);
	if (!bAiming)
	{
		AddControllerPitchInput(ClampedRate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
	else
	{
		AddControllerPitchInput(ClampedRate * AimingLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void AMainCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else
	{
		Super::Jump();
	}
}

void AMainCharacter::StopJumping()
{
	Super::StopJumping();
}

void AMainCharacter::AdjustCameraLengthUp()
{
	if (!CameraBoom) return;

	if (CameraBoom->TargetArmLength <= 00.f)
	{
		return;
	}
	else
	{
		CameraBoom->TargetArmLength -= 30.f;
	}
}

void AMainCharacter::AdjustCameraLengthDown()
{
	if (!CameraBoom)return;
	if (CameraBoom->TargetArmLength >= 670.f)
	{
		return;
	}
	else
	{
		CameraBoom->TargetArmLength += 30.f;
	}
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

void AMainCharacter::PlayGunFireSound()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FireSound);
	}
}

void AMainCharacter::FireOneBullet()
{
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		FVector BeamEnd;
		const bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
		if (bBeamEnd)
		{
			if (ImpactParticles && bNothingHit == false)
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
	}
}

void AMainCharacter::PlayRecoilAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AMainCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo())
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Fired!"));

		//Handles the sound for the gunfire
		PlayGunFireSound();

		// Handles Bullet and finds where it hits
		FireOneBullet();

		//Play Animation for Recoil
		PlayRecoilAnimation();

		// Handles Crosshair Animation
		StartCrossHairBulletFire();

		// Decrease Ammo in weapon by 1
		EquippedWeapon->DecreaseAmmo();

		// Handles the Timer attached to the gun's fire rate
		StartFireTimer();
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

	OutBeamLocation = (WeaponTraceEnd);
	bNothingHit = true;
	return true;
}

void AMainCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_ReloadingState)
	{
		Aim();
	}
}

void AMainCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	if (bAiming)
	{
		StopAiming();
	}
}

void AMainCharacter::Aim()
{
	if (!bAiming && bAimingButtonPressed)
	{
		bAiming = true;
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
	}
}

void AMainCharacter::StopAiming()
{
	if (bAiming)
	{
		bAiming = false;
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
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

void AMainCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation = {WeaponInterpComp, 0};
	InterpLocations.Add(WeaponLocation);

	FInterpLocation ItemLoc1 = {InterpComp1, 0};
	InterpLocations.Add(ItemLoc1);

	FInterpLocation ItemLoc2 = {InterpComp2, 0};
	InterpLocations.Add(ItemLoc2);

	FInterpLocation ItemLoc3 = {InterpComp3, 0};
	InterpLocations.Add(ItemLoc3);

	FInterpLocation ItemLoc4 = {InterpComp4, 0};
	InterpLocations.Add(ItemLoc4);

	FInterpLocation ItemLoc5 = {InterpComp5, 0};
	InterpLocations.Add(ItemLoc5);

	FInterpLocation ItemLoc6 = {InterpComp6, 0};
	InterpLocations.Add(ItemLoc6);
}

int32 AMainCharacter::GetInterpLocationIndex()
{
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;
	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}


	return LowestIndex;
}

void AMainCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	if (Amount != 1) return;
	if (InterpLocations.Num() >= Index)
	{
		InterpLocations[Index].ItemCount += Amount;
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
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();

				if (TraceHitItemLastFrame)
				{
					if (TraceHitItem != TraceHitItemLastFrame)
					{
						TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
							TraceHitItemLastFrame->DisableCustomDepth();
						
						
					}
					
				}

				TraceHitItemLastFrame = TraceHitItem;
			}
			else if (TraceHitItemLastFrame)
			{
				TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				if(TraceHitItemLastFrame->bInterping==false)
				{
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}
		}
	}
}

AWeapon* AMainCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void AMainCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		/*Creates the Skeletal Mesh Socket pointer to spawn the weapon into, called
		 *"RightHandSocket" in Unreal Engine
		 */
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			if (HandSocket)
			{
				HandSocket->AttachActor(WeaponToEquip, GetMesh());
			}
		}
		// Assigning the Equipped Weapon Variable
		EquippedWeapon = WeaponToEquip;

		/* Ensuring that once the weapon is equipped, it doesn't interfere with other
		 *Collision channels	*/
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AMainCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DTR(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DTR);

		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AMainCharacter::TestButtonPressed()
{
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this);
	}
}

void AMainCharacter::TestButtonReleased()
{
}

void AMainCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void AMainCharacter::PickupAmmo(AAmmo* Ammo)
{
	// Does AmmoMap contatin Ammo's AmmoType ?
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		// Get the amount of ammo in our ammo map for this type
		int32 AmmoCount = AmmoMap[Ammo->GetAmmoType()];

		AmmoCount += Ammo->GetItemCount();
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		if (EquippedWeapon->GetAmmo() == 0) { ReloadWeapon(); }
	}

	Ammo->Destroy();
}

void AMainCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_Pistol, StartingPistolAmmo);
	AmmoMap.Add(EAmmoType::EAT_AssaultRifle, StartingAssaultRifleAmmo);
}

bool AMainCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	return (EquippedWeapon->GetAmmo() > 0);
}

void AMainCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AMainCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;


	if (EquippedWeapon == nullptr) return;

	if (CarryingAmmo() && (!EquippedWeapon->ClipIsFull()))
	{
		CombatState = ECombatState::ECS_ReloadingState;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			//TODO : Switch on equipped weapon type
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSection());
		}
		if (bAiming)
		{
			StopAiming();
		}
	}
}

bool AMainCharacter::CarryingAmmo()
{
	if (!EquippedWeapon)return false;

	auto AmmoType = EquippedWeapon->GetAmmoType();
	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}

	return false;
}

void AMainCharacter::FinishReloading()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}

	if (EquippedWeapon == nullptr) return;
	const auto AmmoType = EquippedWeapon->GetAmmoType();
	if (AmmoMap.Contains(AmmoType))
	{
		// Amount of ammo the character is carrrying of the equipped weapon's ammo type
		int32 CarriedAmmo = AmmoMap[AmmoType];

		// How much space is left in the magazine of equipped weapon
		const int32 MagazineRoom = EquippedWeapon->GetMagazineSize() - EquippedWeapon->GetAmmo();

		// Reload the Magazine with as much ammo as we can, if we can't fill it all the way up and set our ammo value
		// in our Ammo Map to 0 
		if (MagazineRoom > CarriedAmmo)
		{
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		// Fill the magazine and update the value stored in our ammo map to reflect it
		else
		{
			EquippedWeapon->ReloadAmmo(MagazineRoom);
			CarriedAmmo -= MagazineRoom;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
	if (bFireButtonPressed)
	{
		FireWeapon();
	}
}

void AMainCharacter::GrabClip()
{
	if (EquippedWeapon == nullptr) return;

	int32 ClipBoneIndex = {EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName())};

	ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);


	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	if (HandClipLocation == nullptr) return;


	HandClipLocation->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("LeftHandSocket")));
	HandClipLocation->SetWorldTransform(ClipTransform);
	EquippedWeapon->SetMovingClip(true);
}

void AMainCharacter::ReleaseClip()
{
	EquippedWeapon->SetMovingClip(false);
}

void AMainCharacter::ToggleSit()
{
	if (MovementStatus == EMovementStatus::EMS_Standing)
	{
		MovementStatus = EMovementStatus::EMS_Sitting;
	}
	else if (MovementStatus == EMovementStatus::EMS_Sitting)
	{
		MovementStatus = EMovementStatus::EMS_Standing;
	}
}

void AMainCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling()) // CombatState != ECombatState::ECS_ReloadingState)
	{
		if (bCrouching)
		{
			bCrouching = false;
			GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
			GetCharacterMovement()->BrakingFriction = 3.f;
			InterpCapsuleHalfHeight();
		}
		else
		{
			bCrouching = true;
			GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
			GetCharacterMovement()->BrakingFriction = 100.f;
			InterpCapsuleHalfHeight();
		}
	}
}

void AMainCharacter::InterpCapsuleHalfHeight()
{
	float TargetHalfHeight = StandingCapsuleHalfHeight;
	if (bCrouching)
	{
		TargetHalfHeight = CrouchingCapsuleHalfHeight;
		const float DeltaCapsuleHalfHeight = TargetHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		const FVector MeshOffset = {0, 0, -DeltaCapsuleHalfHeight};
		GetMesh()->AddLocalOffset(MeshOffset);

		GetCapsuleComponent()->SetCapsuleHalfHeight(TargetHalfHeight, true);
	}
	else
	{
		TargetHalfHeight = StandingCapsuleHalfHeight;
		const float DeltaCapsuleHalfHeight = TargetHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		const FVector MeshOffset = {0, 0, -DeltaCapsuleHalfHeight};
		GetMesh()->AddLocalOffset(MeshOffset);

		GetCapsuleComponent()->SetCapsuleHalfHeight(StandingCapsuleHalfHeight, true);
	}
}

void AMainCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void AMainCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AMainCharacter::StartFireTimer()
{
	CombatState = ECombatState::ECS_FireTimerInProgress;
	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AMainCharacter::AutoFireReset, AutoFireRate);
	bNothingHit = false;
}

void AMainCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed)
		{
			FireWeapon();
		}
	}
	else
	{
		ReloadWeapon();
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

FInterpLocation AMainCharacter::GetInterpLocation(int32 Index)
{
	if (Index <= InterpLocations.Num())
	{
		return InterpLocations[Index];
	}
	return FInterpLocation();
}
