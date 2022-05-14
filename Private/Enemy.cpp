// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"


#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"




// Sets default values
AEnemy::AEnemy() :
Health(100.f),
MaxHealth(100.f),
HealthBarDisplayTime(4.f),
HitNumberDestoryTime(2.5f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	
	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	DrawDebugSphere(GetWorld(), WorldPatrolPoint, 25.f, 12, FColor::Red, true);
	EnemyController = Cast<AEnemyController>(GetController());
	if(EnemyController)
	{
		EnemyController->GetMyBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);

		EnemyController->RunBehaviorTree(BehaviorTree);
		
	}
	
	
	
}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::HideHealthBar, HealthBarDisplayTime);
}

void AEnemy::EnemyDeath()
{
	HideHealthBar();
}

void AEnemy::PlayHitMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance)
	{
		AnimInstance->Montage_Play(HitMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, HitMontage);
	}


	
}

void AEnemy::StoreHitNumer(UUserWidget* HitNumber, FVector Location)
{
	HitNumbers.Add(HitNumber, Location);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;

	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), HitNumber);
	GetWorld()->GetTimerManager().SetTimer(HitNumberTimer, HitNumberDelegate, HitNumberDestoryTime, false);

	
	
}

void AEnemy::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();

	
}

void AEnemy::UpdateHitNumbers()
{
	for (auto& HitPair : HitNumbers)
	{
		UUserWidget* HitNumber = {HitPair.Key};
		const FVector Location = {HitPair.Value};
		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location,ScreenPosition);

		HitNumber->SetPositionInViewport(ScreenPosition);
				
	}
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHitNumbers();

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(FHitResult HitResult)
{
	if(ImpactSound)
	{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}
	ShowHealthBar();

	PlayHitMontage(FName("HitReact1"));
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if(Health-DamageAmount<=0.f)
	{
		Health=0.f;
		GetWorldTimerManager().ClearTimer(HealthBarTimer);
		GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::EnemyDeath, HealthBarDisplayTime);
	}
	else
	{
		Health-=DamageAmount;
		
	}
		return DamageAmount;
	
}

