// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon():
	ThrowWeaponTime(0.7f),
	bFalling(false),
	AmmoCount(30),
	MagazineSize(30),
	WeaponType(EWeaponType::EWT_SubmachineGun),
	AmmoType(EAmmoType::EAT_Pistol),
	ReloadMontageSection(FName(TEXT("Reload_SMG"))),
	ClipBoneName(TEXT("smg_clip")),
	SlideDisplacementTime(.15f),
	bMovingSlide(false),
	MaxSlideDisplacement(3.f),
	MaxRecoilRotation(4.f),
	bAutomatic(true)

{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// Keeps the weapon upright while it is falling
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation = {0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f};
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	UpdateSlideDisplacement();



	
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation = {0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f};
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	// const FVector MeshForward = {GetItemMesh()->GetForwardVector() };
	const FVector MeshRight = {GetItemMesh()->GetRightVector()};


	float RandomRotation = {FMath::RandRange(15.f, 40.f)};

	// The direction in which we throw the weapon ( initialized as a unit vector pointing straight up : )
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(RandomRotation, FVector{0.f, 0.f, 1.f});
	ImpulseDirection *= 20'000.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;

	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::DecreaseAmmo()
{
	if (AmmoCount - 1 <= 0)
	{
		AmmoCount = 0;
	}
	else
	{
		--AmmoCount;
	}
}

void AWeapon::ReloadAmmo(int32 AmmoAmount)
{
	checkf(AmmoCount+AmmoAmount <= MagazineSize, TEXT("Attempted To reload with more than magazine capacity "));
	AmmoCount += AmmoAmount;
}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetItemState(EItemState::EIS_OnGround);
	StartPulseTimer();
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FString DataTablePath = {TEXT("DataTable'/Game/MyStuff/DataTables/WeaponDataTable.WeaponDataTable'")};
	UDataTable* WeaponDataTableObject = Cast<UDataTable>(
		StaticLoadObject(UDataTable::StaticClass(), nullptr, *DataTablePath));

	if (WeaponDataTableObject)
	{
		FWeaponDataTable* WeaponDataRow = nullptr;


		switch (WeaponType)
		{
		case EWeaponType::EWT_SubmachineGun:
			WeaponDataRow = WeaponDataTableObject->FindRow<FWeaponDataTable>(FName("SMG"), TEXT(""));


			break;
		case EWeaponType::EWT_AssaultRifle:
			WeaponDataRow = WeaponDataTableObject->FindRow<FWeaponDataTable>(FName("AR"), TEXT(""));

			break;

		case EWeaponType::EWT_Pistol:
			WeaponDataRow = WeaponDataTableObject->FindRow<FWeaponDataTable>(FName("Pistol"), TEXT(""));

			break;
			
		default:
			;
		}

		if (WeaponDataRow)
		{
			AmmoType = WeaponDataRow->AmmoType;
			AmmoCount = WeaponDataRow->WeaponAmmo;
			MagazineSize = WeaponDataRow->MagazineSize;
			SetPickupSound(WeaponDataRow->PickupSound);
			SetEquipSound(WeaponDataRow->EquipSound);
			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);
			SetItemName(WeaponDataRow->ItemName);
			SetIconItem(WeaponDataRow->InventoryIcon);
			SetAmmoIcon(WeaponDataRow->AmmoIcon);
			SetMaterialInstance(WeaponDataRow->MaterialInstance);

			PreviousMaterialIndex = GetMaterialIndex();
			GetItemMesh()->SetMaterial(PreviousMaterialIndex, nullptr);
			SetMaterialIndex(WeaponDataRow->MaterialIndex);
			SetClipBoneName(WeaponDataRow->ClipBoneName);
			SetReloadMontageSection(WeaponDataRow->ReloadMontageSection);
			GetItemMesh()->SetAnimInstanceClass(WeaponDataRow->AnimBP);
			CrosshairsBottom = WeaponDataRow->CrosshairsBottom;
			CrosshairsLeft = WeaponDataRow->CrosshairsLeft;
			CrosshairsRight = WeaponDataRow->CrosshairsRight;
			CrosshairsTop = WeaponDataRow->CrosshairsTop;
			CrossHairsMiddle = WeaponDataRow->CrossHairsMiddle;
			AutoFireRate = WeaponDataRow->AutoFireRate;
			MuzzleFlash = WeaponDataRow->MuzzleFlash;
			FireSound = WeaponDataRow->FireSound;
			BoneToHide = WeaponDataRow->BoneToHide;
			bAutomatic = WeaponDataRow->bAutomatic;
			Damage = WeaponDataRow->Damage;
			HeadShotDamage = WeaponDataRow->HeadShotDamage;

			
			
		}
		if (GetMaterialInstance())
		{
			SetDynamicMaterialInstance(UMaterialInstanceDynamic::Create(GetMaterialInstance(), this));
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("Fresnel Color"), GetGlowColor());
			GetItemMesh()->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());
			EnableGlowMaterial();
		}
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if(BoneToHide!=FName(""))
	{
		GetItemMesh()->HideBoneByName(BoneToHide, EPhysBodyOp::PBO_None);
	}
	
}

void AWeapon::FinishMovingSlide()
{
	bMovingSlide=false;
}

void AWeapon::UpdateSlideDisplacement()
{
	if(SlideDisplacementCurve && bMovingSlide)
	{
		const float ElapsedTime =GetWorldTimerManager().GetTimerElapsed(SlideTimer);
		const float CurveValue = SlideDisplacementCurve->GetFloatValue(ElapsedTime);
		SlideDisplacement = CurveValue * MaxSlideDisplacement;
		RecoilRoation=CurveValue*MaxRecoilRotation;
	}
}

bool AWeapon::ClipIsFull()
{
	return AmmoCount >= MagazineSize;
}

void AWeapon::StartSlideTimer()
{
	bMovingSlide=true;
	GetWorldTimerManager().SetTimer(SlideTimer, this, &AWeapon::FinishMovingSlide, SlideDisplacementTime);
}
