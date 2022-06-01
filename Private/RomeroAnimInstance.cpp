// Fill out your copyright notice in the Description page of Project Settings.


#include "RomeroAnimInstance.h"

#include "Enemy.h"

URomeroAnimInstance::URomeroAnimInstance():
bDead(false),
LocomotionBlendWeight(.85f)
{
	
}

void URomeroAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if(Enemy == nullptr)
	{
		Enemy=Cast<AEnemy>(TryGetPawnOwner());
	}

	if(Enemy)
	{
		FVector Velocity = { Enemy->GetVelocity()} ;
		Velocity.Z = 0.f;
		Speed=Velocity.Size();

		bCrawling=Enemy->GetCrawling();
		bDead=Enemy->GetDead();
		if(bDead)
		{
			LocomotionBlendWeight = 0.f;
			return;
		}
		if(Enemy->GetAttacking())
		{
			LocomotionBlendWeight=1.f;
		}
		else if (Enemy->GetAttacking() == false)
		{
			LocomotionBlendWeight = .85f;
		}
	}
}
