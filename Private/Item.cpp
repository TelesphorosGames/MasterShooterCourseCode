// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

#include "MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AItem::AItem() : 
bHudDisplayPossible(false)
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

	PickupWidget->SetVisibility(false);

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	//TODO:  Ensure only the correct HUD element is being drawn for the item being overlapped 
	if(OtherActor)
	{
		AMainCharacter* ShooterCharacter = Cast<AMainCharacter>(OtherActor);

		if (ShooterCharacter)
		{
			bHudDisplayPossible = false;
			ShooterCharacter->IncrementOverlappedItemCount(-1);
			if(ShooterCharacter->GetItemBeingLookedAt()==this)
			{
				PickupWidget->SetVisibility(false);
				ShooterCharacter->bItemHudCurrentlyDisplayed = false;
			}
			if(ShooterCharacter->GetOverlappedItemCount()<=0&&ShooterCharacter->GetItemBeingLookedAt()!=nullptr)
			{
				ShooterCharacter->GetItemBeingLookedAt()->PickupWidget->SetVisibility(false);
				ShooterCharacter->bItemHudCurrentlyDisplayed = false;
			}
		}
	}
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* ShooterCharacter = Cast<AMainCharacter>(OtherActor);

		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
			bHudDisplayPossible = true;
		}
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
