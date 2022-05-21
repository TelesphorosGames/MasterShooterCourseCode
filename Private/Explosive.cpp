// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"

#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AExplosive::AExplosive()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	ExplosiveMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosivesMesh"));
	SetRootComponent(ExplosiveMesh);

	ExplosiveRangeSphere=CreateDefaultSubobject<USphereComponent>(TEXT("ExplosiveRange"));
	ExplosiveRangeSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AExplosive::BeginPlay()
{
	Super::BeginPlay();

	

	
}

// Called every frame
void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosive::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}

	TArray<AActor*> ExplodingActors;
	ExplosiveRangeSphere->GetOverlappingActors(ExplodingActors, ACharacter::StaticClass());

	for(auto Enemies : ExplodingActors)
	{
		UGameplayStatics::ApplyDamage(Enemies, 65.f, ShooterController, this, UDamageType::StaticClass());
	}
	
	Destroy();
}

