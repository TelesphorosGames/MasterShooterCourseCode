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

	bNothingHit(false),
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
	CrouchMovementSpeed(225.f),
	BaseMovementSpeed(675.f),
	CurrentCapsuleHalfHeight(88.f),
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	BaseTurnRate(105.f),
	BaseLookUpRate(95.f),
	HipTurnRate(105.f),
	HipLookUpRate(95.f),
	AimingTurnRate(25.f),
	AimingLookUpRate(25.f),
	bShouldFire(true),
	HighlightedSlot(-1),
	WeaponInExchange(nullptr)

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

	PlayerInputComponent->BindAction("GKey", IE_Pressed, this, &AMainCharacter::GKeyPressed);
	PlayerInputComponent->BindAction("1Key", IE_Pressed, this, &AMainCharacter::OneKeyPressed);
	PlayerInputComponent->BindAction("2Key", IE_Pressed, this, &AMainCharacter::TwoKeyPressed);
	PlayerInputComponent->BindAction("3Key", IE_Pressed, this, &AMainCharacter::ThreeKeyPressed);
	PlayerInputComponent->BindAction("4Key", IE_Pressed, this, &AMainCharacter::FourKeyPressed);
	PlayerInputComponent->BindAction("5Key", IE_Pressed, this, &AMainCharacter::FiveKeyPressed);

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
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);
	EquippedWeapon->DisableGlowMaterial();
	EquippedWeapon->GetItemMesh()->bCastDynamicShadow = true;
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
		if (Inventory.Num() < InventorySize)
		{
			Weapon->SetSlotIndex(Inventory.Num());
			AttachWeaponToSocket(Weapon);
			Inventory.Add(Weapon);
			CombatState = ECombatState::ECS_Unoccupied;
			Weapon->SetItemState(EItemState::EIS_PickedUp);
		}
		else
		{
			SwapWeapon(Weapon);
		}
	}

	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo)
	{
		PickupAmmo(Ammo);
	}
	Item->DisableCustomDepth();
	if (bFireButtonPressed)
	{
		FireWeapon();
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
	if (EquippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), EquippedWeapon->GetFireSound());
	}
}

void AMainCharacter::FireOneBullet()
{
	if (CombatState == ECombatState::ECS_Unoccupied)
	{
		const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");
		if (BarrelSocket)
		{
			const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());
			if (EquippedWeapon->GetMuzzleFlash())
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquippedWeapon->GetMuzzleFlash(), SocketTransform);
			}

			FVector BeamEnd;

			if(bCrouching)
			{
				BeamEnd = CrossHairPublicHit;
			}
			else
			{
				GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
			}
			

			if (ImpactParticles && !bNothingHit)
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

void AMainCharacter::AttachWeaponToSocket(AWeapon* Weapon)
{
	FAttachmentTransformRules Fatr(FAttachmentTransformRules::SnapToTargetIncludingScale);

	if (Weapon->GetSlotIndex() == 0)
	{
		Weapon->AttachToComponent(GetMesh(), Fatr, FName("GunSocket3"));

		return;
	}
	if (Weapon->GetSlotIndex() == 1)
	{
		Weapon->AttachToComponent(GetMesh(), Fatr, FName("GunSocket1"));

		return;
	}
	if (Weapon->GetSlotIndex() == 2)
	{
		Weapon->AttachToComponent(GetMesh(), Fatr, FName("GunSocket2"));

		return;
	}
	if (Weapon->GetSlotIndex() == 3)
	{
		Weapon->AttachToComponent(GetMesh(), Fatr, FName("GunSocket3"));

		return;
	}
	if (Weapon->GetSlotIndex() == 4)
	{
		Weapon->AttachToComponent(GetMesh(), Fatr, FName("GunSocket4"));

		return;
	}
	if (Weapon->GetSlotIndex() == 5)
	{
		Weapon->AttachToComponent(GetMesh(), Fatr, FName("GunSocket5"));

		return;
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

		if(EquippedWeapon->GetWeaponType()==EWeaponType::EWT_Pistol)
		{
			EquippedWeapon->StartSlideTimer();
		}
	}
}


bool AMainCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	FHitResult CrosshairHitResult;

	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		OutBeamLocation = CrosshairHitResult.Location;
		CrossHairPublicHit = CrosshairHitResult.Location;
		
	}

	FHitResult WeaponTraceHit;

	const FVector WeaponTraceStart = MuzzleSocketLocation;

	const FVector WeaponTraceEnd = EquippedWeapon->GetItemMesh()->GetChildComponent(6)->GetComponentLocation();

	GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECC_Visibility);
	if (WeaponTraceHit.bBlockingHit)
	{
		OutBeamLocation = WeaponTraceHit.Location;
		bNothingHit = false;
		
		return true;
	}

	OutBeamLocation = BeamEndPublic;
	bNothingHit = true;
	return true;
}

void AMainCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_ReloadingState && CombatState != ECombatState::ECS_PickingUpWeapon)
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
		bShouldTraceForItems = true;
	}
}

void AMainCharacter::StopAiming()
{
	if (bAiming)
	{
		bAiming = false;
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
		bShouldTraceForItems = false;
		if (GetItemBeingLookedAt())
		{
			if (GetItemBeingLookedAt()->GetPickupWidget())
			{
				GetItemBeingLookedAt()->GetPickupWidget()->SetVisibility(false);
				if (GetItemBeingLookedAt()->bInterping == false)
				{
					GetItemBeingLookedAt()->DisableCustomDepth();
				}
			}
		}
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
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 1.f, DeltaTime, 10.f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 10.f);
	}

	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, .5f, DeltaTime, 10.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 10.f);
	}

	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 1.f, DeltaTime, 10.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 10.f);
	}

	if(bCrouching)
	{
		CrosshairCrouchingFactor = FMath::FInterpTo(CrosshairCrouchingFactor, 1.f, DeltaTime, 10.f);
	}
	else
	{
		CrosshairCrouchingFactor = FMath::FInterpTo(CrosshairCrouchingFactor, 0.f, DeltaTime, 10.f);
	}

	CrosshairSpreadMultiplier = 1.f + CrosshairVelocityFactor/1.5  + CrosshairInAirFactor - CrosshairAimFactor +
		CrosshairShootingFactor - CrosshairCrouchingFactor ;
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
			auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
			if (TraceHitWeapon && TraceHitWeapon != EquippedWeapon)
			{
				if (HighlightedSlot == -1)
				{
					HighlightInventorySlot();
				}
			}
			else
			{
				if (HighlightedSlot != -1)
				{
					UnHighlightInventorySlot();
				}
			}


			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
			{
				TraceHitItem = nullptr;
			}
			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				FVector TraceHitItemLoc = TraceHitItem->GetActorLocation();
				FVector CharacterLoc = GetActorLocation();
				double DistanceToItem = UKismetMathLibrary::Vector_Distance(CharacterLoc, TraceHitItemLoc);

				int32 DistanceInt = UKismetMathLibrary::FCeil(DistanceToItem);
				TraceHitItem->DistanceToCharacter = (DistanceInt / 100);
				if (TraceHitItem->bIsOverlappingChar == true || bAiming && TraceHitItem != EquippedWeapon)
				{
					TraceHitItem->GetPickupWidget()->SetVisibility(true);
					TraceHitItem->EnableCustomDepth();

					if (Inventory.Num() >= InventorySize)
					{
						TraceHitItem->SetCharacterInventoryFull(true);
					}
					else
					{
						TraceHitItem->SetCharacterInventoryFull(false);
					}

					if (TraceHitItemLastFrame)
					{
						if (TraceHitItem != TraceHitItemLastFrame)
						{
							TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
							if (TraceHitItemLastFrame->bInterping == false)
							{
								TraceHitItemLastFrame->DisableCustomDepth();
							}
						}
					}

					TraceHitItemLastFrame = TraceHitItem;
				}
			}
			else if (TraceHitItemLastFrame)
			{
				TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				if (TraceHitItemLastFrame->bInterping == false)
				{
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}
		}
		else
		{
			TraceHitItem = nullptr;
			if (TraceHitItemLastFrame && TraceHitItemLastFrame != TraceHitItem)
			{
				TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				if (TraceHitItemLastFrame->bInterping == false)
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
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("SMG_Socket"));
		const USkeletalMeshSocket* ARHandSocket = GetMesh()->GetSocketByName(FName("AR_Socket"));
		switch (WeaponToEquip->GetWeaponType())
		{
		case EWeaponType::EWT_SubmachineGun:
			if (HandSocket)
			{
				HandSocket->AttachActor(WeaponToEquip, GetMesh());
			}
			break;

		case EWeaponType::EWT_AssaultRifle:
			if (ARHandSocket)
			{
				ARHandSocket->AttachActor(WeaponToEquip, GetMesh());
			}
		case EWeaponType::EWT_Pistol:
			if (HandSocket)
			{
				HandSocket->AttachActor(WeaponToEquip, GetMesh());
			}
			break;
		
		default:
			;
		}


		//Brodcasting the current slot index and the new slot index to the inventory bar widget
		// -1 = no equipped weapon yet, no need to play ( reverse ) item animation
		if (EquippedWeapon == nullptr)
		{
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}
		else
		{
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
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
		EquippedWeapon->EnableGlowMaterial();

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && EquipMontage)
		{
			AnimInstance->Montage_Play(EquipMontage);
			AnimInstance->Montage_JumpToSection(FName("ThrowWeapon"));
		}
	}
}

void AMainCharacter::TestButtonPressed()
{
	if (bAiming)return;
	if (TraceHitItem)
	{
		if (CombatState == ECombatState::ECS_Unoccupied)
		{
			AWeapon* WeaponHit = Cast<AWeapon>(TraceHitItem);
			if (WeaponHit)
			{
				if (WeaponHit->bIsOverlappingChar)
				{
					WeaponHit->StartItemCurve(this);
					CombatState = ECombatState::ECS_PickingUpWeapon;
					TraceHitItem = nullptr;
				}
			}
			else
			{
				AAmmo* AmmoHit = Cast<AAmmo>(TraceHitItem);
				if (AmmoHit)
				{
					CombatState = ECombatState::ECS_Unoccupied;
					if (AmmoHit->bIsOverlappingChar)
					{
						AmmoHit->StartItemCurve(this);
					}
				}
			}
		}
	}
}

void AMainCharacter::TestButtonReleased()
{
}

void AMainCharacter::GKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 0) return;
	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 0);
}

void AMainCharacter::OneKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 1) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);
}

void AMainCharacter::TwoKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 2) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 2);
}

void AMainCharacter::ThreeKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 3) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 3);
}

void AMainCharacter::FourKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 4) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 4);
}

void AMainCharacter::FiveKeyPressed()
{
	if (EquippedWeapon->GetSlotIndex() == 5) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 5);
}

void AMainCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	if (Inventory.Num() - 1 >= EquippedWeapon->GetSlotIndex())
	{
		Inventory[EquippedWeapon->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquippedWeapon->GetSlotIndex());
	}
	DropWeapon();
	EquippedWeapon->GetItemMesh()->bCastDynamicShadow = false;
	EquipWeapon(WeaponToSwap);
	CombatState = ECombatState::ECS_Unoccupied;
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
	WeaponToSwap->GetItemMesh()->bCastDynamicShadow = true;
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
	if (bFireButtonPressed)
	{
		FireWeapon();
	}
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
	if (EquippedWeapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipped weapon is null here bro"));
		return;
	}
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

	if (CombatState == ECombatState::ECS_PickingUpWeapon) return;
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}
	if (bFireButtonPressed)
	{
		FireWeapon();
	}
}

void AMainCharacter::FinishEquipping()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if(bAimingButtonPressed)
	{
		Aim();
	}
}

void AMainCharacter::FinishDisarming()
{
	WeaponInExchange = nullptr;
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

void AMainCharacter::ExchangeInventoryItems(int32 CurrentIndex, int32 NewItemIndex)
{
	if (CurrentIndex == NewItemIndex || NewItemIndex >= Inventory.Num()) return;
	if (CombatState == ECombatState::ECS_Unoccupied)
	{
		if(bAiming) StopAiming();
		
		WeaponInExchange = EquippedWeapon;
		auto OldEquippedWeapon = EquippedWeapon;
		auto NewWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && EquipMontage)
		{
			int32 Rand = FMath::RandRange(1, 2);
			switch (Rand)
			{
			case 1:
				AnimInstance->Montage_Play(EquipMontage, 1.0f);
				AnimInstance->Montage_JumpToSection(FName("Equip"));
				break;

			case 2:
				AnimInstance->Montage_Play(EquipMontage, 1.0f);
				AnimInstance->Montage_JumpToSection(FName("Equip2"));

				break;

			default:
				;
			}
		}


		OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
		NewWeapon->SetItemState(EItemState::EIS_Equipped);

		CombatState = ECombatState::ECS_PickingUpWeapon;
		EquipWeapon(NewWeapon);
	}
}

int32 AMainCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			return i;
		}
	}
	if (Inventory.Num() < InventorySize)
	{
		return Inventory.Num();
	}


	return -1;
}

void AMainCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot = GetEmptyInventorySlot();

	HighlightIconDelegate.Broadcast(EmptySlot, true);
	HighlightedSlot = EmptySlot;
}

void AMainCharacter::UnHighlightInventorySlot()
{
	HighlightIconDelegate.Broadcast(HighlightedSlot, false);
	HighlightedSlot = -1;
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
	if (!EquippedWeapon)return;
	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AMainCharacter::AutoFireReset,
	                                EquippedWeapon->GetAutoFireRate());

	bNothingHit = false;
}

void AMainCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (EquippedWeapon && WeaponHasAmmo())
	{
		if (bFireButtonPressed && EquippedWeapon->GetAutomatic())
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
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	CrosshairLocation = {(ViewportSize.X / 2.f), (ViewportSize.Y / 2.f)};

	FVector CrossHairWorldPosition;
	FVector CrossHairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
	                                                               CrosshairLocation, CrossHairWorldPosition,
	                                                               CrossHairWorldDirection);
	if (bScreenToWorld)
	{
		//Trace from Crosshair World Location Outward - Ray cast to see if any item collisions occur 
		const FVector Start = {CrossHairWorldPosition};
		FVector End = Start + CrossHairWorldDirection * 50000;
		BeamEndPublic = End;
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


/* ************************** DISCARDED CODE *******************************************
*

//**** PERTAINING TO GETBEAMENDLOCATION() ****************
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

	// FHitResult FireHitResult;
	// const FVector Start = MuzzleSocketLocation;
	// const FRotator Rotation = GetEquippedWeapon()->GetActorRotation();
	// const FVector RoationAxis = Rotation.Vector();
	// const FVector End = MuzzleSocketLocation -WeaponTraceEnd * 50000;
	//
	// FVector BeamEndPoint = End;
	//
	// GetWorld()->LineTraceSingleByChannel(FireHitResult, Start, End, ECollisionChannel::ECC_Visibility);
	// if(FireHitResult.bBlockingHit)
	// {
	// 	BeamEndPoint = FireHitResult.Location;
	// 	DrawDebugLine(GetWorld(), Start, End, FColor::Emerald, false, 3.f);
	// 	DrawDebugPoint(GetWorld(), FireHitResult.Location, 10, FColor::Red, false, 3.5f);
	// 	OutBeamLocation = BeamEndPoint;
	// 	return true;
	// }

		// const FVector Start = {CrossHairWorldPosition};
		// FVector End = Start +  CrossHairWorldDirection * 50000;
		// FVector End = {Start + CrossHairWorldDirection * 50000};
 *
 *
*
		// *** USE TO CREATE FSTRINGS WITH Printf AND GEngine->AddOnscreenDebugMessage
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
/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
*/
