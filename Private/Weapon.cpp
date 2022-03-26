// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon():
ThrowWeaponTime(0.7f),
bFalling(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// Keeps the weapon upright while it is falling
	if(GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation = {0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f};
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation = {0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f};
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward = {GetItemMesh()->GetForwardVector() };
	const FVector MeshRight = {GetItemMesh()->GetRightVector() };


	float RandomRotation = { FMath::RandRange(15.f, 40.f) };

	// The direction in which we throw the weapon ( initialized as a unit vector pointing straight up : )
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(RandomRotation, FVector{0.f, 0.f, 1.f});
	ImpulseDirection *= 20'000.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;

	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::StopFalling()
{
	bFalling=false;
	SetItemState(EItemState::EIS_OnGround);
}
