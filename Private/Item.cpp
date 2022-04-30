// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Curves/CurveVector.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
AItem::AItem() :
	ItemName(FString("Default")),
	ItemCount(0),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState(EItemState::EIS_OnGround),
	ZCurveInterpTime(1.f),
	ItemType(EItemType::EIT_MAX),
	InterpLocIndex(0),
	MaterialIndex(0),
	PulseCurveTime(2.5f),
	GlowAmount(0.f),
	FresnelExponent(0.f),
	FresnelReflectFraction(0.f),
	SlotIndex(0),
	bCharacterInventoryFull(false)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);

	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	// Hide Pickup Widget initially

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}


	SetActiveStars();

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	// Sets initial items on ground to their default, on ground Item state, ready to be picked up
	SetItemProperties(ItemState);

	InitializeCustomDepth();

	StartPulseTimer();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ItemInterp(DeltaTime);
	UpdatePulseEffect();
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* ShooterCharacter = Cast<AMainCharacter>(OtherActor);

		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(-1);
			ShooterCharacter->UnHighlightInventorySlot();
			if (ShooterCharacter->GetItemBeingLookedAt() == this)
			{
				PickupWidget->SetVisibility(false);
			}
			if (ShooterCharacter->GetOverlappedItemCount() <= 0 || ShooterCharacter->GetItemBeingLookedAt() != nullptr)
			{
				if (ShooterCharacter->GetItemBeingLookedAt())
				{
					if (ShooterCharacter->GetItemBeingLookedAt()->GetPickupWidget())
					{
						ShooterCharacter->GetItemBeingLookedAt()->GetPickupWidget()->SetVisibility(false);
					}
					if (ShooterCharacter->GetItemBeingLookedAt()->bInterping == false)
					{
						ShooterCharacter->GetItemBeingLookedAt()->DisableCustomDepth();
					}
				}
			}
			bIsOverlappingChar = false;
		}
	}
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                            const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* ShooterCharacter = Cast<AMainCharacter>(OtherActor);

		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
			bIsOverlappingChar = true;
		}
	}
}

void AItem::SetActiveStars()
{
	for (int32 i = 0; i <= 5; i++)
	{
		ActiveStars.Add(false);
	}

	switch (ItemRarity)
	{
	case EItemRarity::EIR_Common:
		ActiveStars[1] = true;
		break;
	case EItemRarity::EIR_Great:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;
	case EItemRarity::EIR_Superb:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_Epic:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;


	default: ;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_OnGround:
		if (ItemMesh)
		{
			GetItemMesh()->SetEnableGravity(false);
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		AreaSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);


		break;

	case EItemState::EIS_Equipped:

		if (PickupWidget)
		{
			PickupWidget->SetVisibility(false);
		}


		if (ItemMesh)
		{
			GetItemMesh()->SetEnableGravity(false);
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		break;

	case EItemState::EIS_Falling:

		if (ItemMesh)
		{
			GetItemMesh()->SetEnableGravity(true);
			GetItemMesh()->SetSimulatePhysics(true);
			GetItemMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GetItemMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
			GetItemMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
			GetItemMesh()->SetVisibility(true);
		}


		AreaSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		break;

	case EItemState::EIS_EquipInterping:

		if (PickupWidget)
		{
			PickupWidget->SetVisibility(false);
		}

		if (ItemMesh)
		{
			GetItemMesh()->SetEnableGravity(false);
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		break;

	case EItemState::EIS_PickedUp:


		if (PickupWidget)
		{
			PickupWidget->SetVisibility(false);
		}

		if (ItemMesh)
		{
			GetItemMesh()->SetEnableGravity(false);
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetVisibility(true);
			ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			//TODO : SOCKET WEAPON TO CHARACTER
		}

		AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		break;


	default: ;
	}
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (CharacterPointer)
	{
		// Resetting the struct for the InterpLocation
		CharacterPointer->IncrementInterpLocItemCount(InterpLocIndex, -1);
		CharacterPointer->GetPickupItem(this);
		CharacterPointer->UnHighlightInventorySlot();
	}
	SetActorScale3D(FVector(1.f));
	DisableGlowMaterial();
	DisableCustomDepth();
}

FVector AItem::GetInterpLocation()
{
	if (CharacterPointer == nullptr) return FVector(0);

	switch (ItemType)
	{
	case EItemType::EIT_Ammo:
		return CharacterPointer->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();

	case EItemType::EIT_Weapon:
		return CharacterPointer->GetInterpLocation(0).SceneComponent->GetComponentLocation();

	default: ;
	}


	return {};
}


void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::StartItemCurve(AMainCharacter* Character)
{
	if (ItemState != EItemState::EIS_OnGround) return;
	GetWorldTimerManager().ClearTimer(PulseTimer);

	SetItemState(EItemState::EIS_EquipInterping);
	GetWorldTimerManager().SetTimer(ItemPickupInterpTimer, this, &AItem::FinishInterping, ZCurveInterpTime);
	CharacterPointer = Character;


	// Get array index in InterLocations with lowest item count
	InterpLocIndex = Character->GetInterpLocationIndex();
	// Adds 1 to the item count for this InterpLocation struct - meaning this spot is full, no other item can go here
	Character->IncrementInterpLocItemCount(InterpLocIndex, 1);

	if (PickupSound)
	{
		UGameplayStatics::PlaySound2D(this, PickupSound);
	}
	// Stores initial location of item when interping begins
	ItemInterpStartLocation = GetActorLocation();
	bInterping = true;
	EnableCustomDepth();


	//* Getting the initial values for the Yaw directions of the item we're picking up
	//* and player camera so that we can match the two and the item always faces the item


	// const double CameraRotationYaw = {Character->GetFollowCamera()->GetComponentRotation().Yaw};
	// const double ItemRotationYaw = {GetActorRotation().Yaw};

	// Inital yaw direction offset between camera and item, used to interp in ItemInterp
	// InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;
}

void AItem::ResetPulseTimer()
{
	StartPulseTimer();
}

void AItem::StartPulseTimer()
{
	if (ItemState == EItemState::EIS_OnGround)
	{
		GetWorldTimerManager().SetTimer(PulseTimer, this, &AItem::ResetPulseTimer, PulseCurveTime);
	}
}


void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping) return;

	if (CharacterPointer && ItemZCurve)
	{
		// Elapsed time since we started Timer for interping item into inventory
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemPickupInterpTimer);

		// Get Curve value corresponding to elapsed time
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);

		FVector CurrentItemLocation = ItemInterpStartLocation;
		const FVector CameraInterpLocation = GetInterpLocation();

		// Vector from item to target location, pointing straight up ( only interping the Z with this curve)

		const FVector ItemToCamera = FVector{0, 0, (CameraInterpLocation - CurrentItemLocation).Z};

		// Scale factor to multiply with curve value
		const float DeltaZ = ItemToCamera.Size();

		const FVector CurrentLocation = GetActorLocation();

		// Used to interpolate the X and Y positions of the item
		const float InterpXValue = FMath::FInterpTo(CurrentLocation.X, CameraInterpLocation.X, DeltaTime, 1.f);
		const float InterpYValue = FMath::FInterpTo(CurrentLocation.Y, CameraInterpLocation.Y, DeltaTime, 40.f);

		CurrentItemLocation.X = InterpXValue;
		CurrentItemLocation.Y = InterpYValue;

		// Adding curve value to Z component of item's location ( scaled by Delta Z )
		CurrentItemLocation.Z += CurveValue * DeltaZ;

		// Updates the item's location - does the interpolation
		SetActorLocation(CurrentItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		// Camera's rotation this frame
		const FRotator CameraCurrentRotation = {CharacterPointer->GetFollowCamera()->GetComponentRotation()};

		// Camera rotation to match the item's interp to 
		FRotator ItemRotation = {CameraCurrentRotation.Pitch, CameraCurrentRotation.Yaw, 0};

		//+ InterpInitialYawOffset
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);


		if (ItemScaleCurve)
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}
}

void AItem::EnableCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(true);
}

void AItem::DisableCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(false);
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}

void AItem::OnConstruction(const FTransform& Transform)
{
	/* Load Data from ItemRarity data table
	*/
	//Path to the actual data table : Apparently important to not change after set here. ?? I am guessing
	FString RarityTablePath(TEXT("DataTable'/Game/MyStuff/DataTables/ItemRarityTable.ItemRarityTable'"));
	UDataTable* RarityTableObject = Cast<UDataTable>(
		StaticLoadObject(UDataTable::StaticClass(), nullptr, *RarityTablePath));
	if (RarityTableObject)
	{
		FItemRarityTable* RarityRow = nullptr;
		switch (ItemRarity)
		{
		case EItemRarity::EIR_Common:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Common"), TEXT(""));

			break;
		case EItemRarity::EIR_Great:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Great"), TEXT(""));

			break;
		case EItemRarity::EIR_Rare:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Rare"), TEXT(""));

			break;
		case EItemRarity::EIR_Superb:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Superb"), TEXT(""));

			break;
		case EItemRarity::EIR_Epic:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Epic"), TEXT(""));

			break;


		default:
			;
		}
		if (RarityRow)
		{
			RowGlowColor = RarityRow->GlowColor;
			RowLightColor = RarityRow->LightColor;
			RowDarkColor = RarityRow->DarkColor;
			RowNumberOfStars = RarityRow->NumberOfStars;
			IconBackground = RarityRow->IconBackground;
			if (GetItemMesh())
			{
				GetItemMesh()->SetCustomDepthStencilValue(RarityRow->CustomDepthStencil);
			}
		}
	}
	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("Fresnel Color"), RowGlowColor);
		ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);
		EnableGlowMaterial();
	}
}

void AItem::UpdatePulseEffect()
{
	float ElapsedTime;
	FVector CurveVaule;

	switch (ItemState)
	{
	case(EItemState::EIS_OnGround):

		if (PulseCurve && DynamicMaterialInstance)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);

			CurveVaule = PulseCurve->GetVectorValue(ElapsedTime);
		}

		break;

	case(EItemState::EIS_EquipInterping):

		if (MaterialInterpPulseCurve && DynamicMaterialInstance)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemPickupInterpTimer);
			CurveVaule = MaterialInterpPulseCurve->GetVectorValue(ElapsedTime);
		}

		break;

	default: ;
	}
	if (DynamicMaterialInstance)
	{
		GlowAmount = 10.f;
		FresnelExponent = 8.f;
		FresnelReflectFraction = .005f;
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("Glow Amount"), CurveVaule.X * GlowAmount);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("Fresnel Exponent"), CurveVaule.Y * FresnelExponent);
		DynamicMaterialInstance->SetScalarParameterValue(
			TEXT("ReflectFractionIn"), CurveVaule.Z * FresnelReflectFraction);
	}
}

void AItem::EnableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0.f);
	}
}

void AItem::DisableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1.f);
	}
}
