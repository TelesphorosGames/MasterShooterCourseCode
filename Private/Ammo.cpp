// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

AAmmo::AAmmo() 
{
	AmmoMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);

	GetCollisionBox()->SetupAttachment(GetRootComponent());
	GetPickupWidget()->SetupAttachment(GetRootComponent());
	GetAreaSphere()->SetupAttachment(GetRootComponent());

	AmmoCollisionSphere=CreateDefaultSubobject<USphereComponent>(TEXT("AmmoCollisionSphere"));
	AmmoCollisionSphere->SetupAttachment(GetRootComponent());
	AmmoCollisionSphere->SetSphereRadius(50.f);
}

void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();
	SetItemProperties(GetItemState());

	AmmoCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::AmmoSphereOverlap);
}

void AAmmo::SetItemProperties(EItemState State)
{
	Super::SetItemProperties(State);

	switch(State)
	{
	case EItemState::EIS_OnGround:
		if(AmmoMesh)
		{
			GetAmmoMesh()->SetEnableGravity(false);
			GetAmmoMesh()->SetSimulatePhysics(false);
            GetAmmoMesh()->SetVisibility(true);
            GetAmmoMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
			GetAmmoMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		break;

	case EItemState::EIS_Equipped:

		if(GetPickupWidget()!=nullptr)
		{
			GetPickupWidget()->SetVisibility(false);
		}
		
		
		if(AmmoMesh)
		{
			GetAmmoMesh()->SetEnableGravity(false);
			GetAmmoMesh()->SetSimulatePhysics(false);
			GetAmmoMesh()->SetVisibility(true);
			GetAmmoMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
			GetAmmoMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}


		break;

	case EItemState::EIS_Falling:

		if(AmmoMesh)
		{
			GetAmmoMesh()->SetEnableGravity(true);
			GetAmmoMesh()->SetSimulatePhysics(true);
            GetAmmoMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            GetAmmoMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
            GetAmmoMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
			
	
		}
		
		break;

	case EItemState::EIS_EquipInterping:

		if(GetPickupWidget() != nullptr)
		{
			GetPickupWidget()->SetVisibility(false);
			
		}
		
		if(AmmoMesh)
		{
			GetAmmoMesh()->SetEnableGravity(false);
			GetAmmoMesh()->SetSimulatePhysics(false);
			GetAmmoMesh()->SetVisibility(true);
			GetAmmoMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
			GetAmmoMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		
		break;

		
		
	default: ;
	}
}

void AAmmo::AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		auto Character = Cast<AMainCharacter>(OtherActor);
		if(Character)
		{
			StartItemCurve(Character);
			AmmoCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}
