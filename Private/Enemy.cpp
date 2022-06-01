// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"


#include "EnemyController.h"
#include "MainCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"




// Sets default values
AEnemy::AEnemy() :
Health(100.f),
MaxHealth(100.f),
HealthBarDisplayTime(4.f),
HitNumberDestoryTime(2.5f),
bStunned(false),
StunnedChance(.5f),
Attack1(TEXT("Attack1")),
Attack2(TEXT("Attack2")),
BaseDamage(2.f),
bCrawling(false),
bDead(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
	AttackRangeSphere->SetupAttachment(GetRootComponent());

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponCollision"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftHandSocket"));
	
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponCollision"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightHandSocket"));
	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	
	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);
	// DrawDebugSphere(GetWorld(), WorldPatrolPoint, 25.f, 12, FColor::Red, true);
	EnemyController = Cast<AEnemyController>(GetController());
	if(EnemyController)
	{
		EnemyController->GetMyBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyController->GetMyBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);

		EnemyController->RunBehaviorTree(BehaviorTree);
		
	}

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);
	
	AttackRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackRangeSphereOverlap);
	AttackRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AttackRangeEndOverlap);

	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);
	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);

	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	
	
		GetCharacterMovement()->MaxWalkSpeed=FMath::RandRange(20,950);
	
	
}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::HideHealthBar, HealthBarDisplayTime);
}

void AEnemy::EnemyDeath()
{
	HideHealthBar();
	Destroy();
	
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

void AEnemy::PlayAttackMontage(FName Section, float PlayRate)
{
	bAttacking=true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance)
	{
		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, AttackMontage);
		
	}
}

FName AEnemy::GetAttackSectionName()
{
	
	FName SectionName;
	const int32 Section = FMath::RandRange(0,2);
	switch(Section)
	{
	case 0:
		SectionName=Attack1;
		break;
	
	case 1:
		SectionName= Attack2;
	break;
	case 2:
		SectionName=FName("Zombie_Bite");

	default: ;
	}

	return SectionName;
	
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

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		
		AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
		if(Character)
		{			
				EnemyController->GetMyBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
		}
	}
}

void AEnemy::SetStunned(bool Stunned)
{
	bStunned = Stunned;

	if(EnemyController)
	{
		EnemyController->GetMyBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), Stunned);
	}
	
}

void AEnemy::AttackRangeSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if(!OtherActor) return;
	{
		AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
		if(Character)
		{
			bInAttackRange = true;

			if(EnemyController)
			{
				EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
			}
		}
	}
}

void AEnemy::AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if(!OtherActor) return;
	{
		AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
		if(Character)
		{
			bInAttackRange = false;
            
            if(EnemyController)
            {
            	EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
            }
		}
	}
	
	
}

void AEnemy::DoDamage(AActor* Victim)
{
	if(Victim)
	{
		AMainCharacter* Char = Cast<AMainCharacter>(Victim);
		
		if(Char)
		{
			UGameplayStatics::ApplyDamage(Char, BaseDamage, EnemyController, this, UDamageType::StaticClass());
			
			UE_LOG(LogTemp,Warning,TEXT("BOOM!"));
			if(Char->GetCharacterDamagedSound())
			{
				Char->StartCharacterDamagedSoundTimer();
			}

			StunCharacterAttempt(Char);
			
		}
	}
}

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DoDamage(OtherActor);
}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DoDamage(OtherActor);
}

void AEnemy::ActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}



void AEnemy::StunCharacterAttempt(AMainCharacter* Victim)
{
	if(Victim && !Victim->GetCharacterDead())
	{
		Victim->PlayHitReact();
		const float Stun = FMath::FRandRange(0.f,1.f);
		if(Stun<=Victim->GetStunChance())
		{
			Victim->Stun();
		}
	}
}

void AEnemy::StopAttacking()
{
	bAttacking=false;
}

void AEnemy::StartEnemyRespawnTimer()
{
	GetWorldTimerManager().SetTimer(EnemyRespawnTimer, this, &AEnemy::SpawnEnemy, EnemyRespawnTime);
}

void AEnemy::SpawnEnemy()
{
	
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

void AEnemy::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	if(ImpactSound)
	{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);
	}
	

	
	
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if(Health-DamageAmount<=0.f)
	{
		Health=0.f;
		GetCharacterMovement()->MaxWalkSpeed=0.f;
		GetWorldTimerManager().ClearTimer(HealthBarTimer);
		GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::EnemyDeath, 30.f);
		bDead=true;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance && DeathMontage)
		{
			int32 Section = FMath::RandRange(0,2);
			switch(Section)
			{
			case 0:
				AnimInstance->Montage_Play(DeathMontage, 1);
				AnimInstance->Montage_JumpToSection(FName("Death1"), DeathMontage);
				break;
			case 1:
				AnimInstance->Montage_Play(DeathMontage, 1);
				AnimInstance->Montage_JumpToSection(FName("Death2"), DeathMontage);
				break;
			case 2:
				AnimInstance->Montage_Play(DeathMontage, 1);
				AnimInstance->Montage_JumpToSection(FName("Death3"), DeathMontage);
				break;
					
			default:;
				
			}
			
		
		}
		
		EnemyController->StopMovement();
		EnemyController->GetMyBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	
	}
	else
	{
		if(!EnemyController || bDead) return 0.f;
		EnemyController->GetMyBlackboardComponent()->SetValueAsObject(FName("Target"), DamageCauser);
		Health-=DamageAmount;

		ShowHealthBar();

		const float Stunned = FMath::FRandRange(0.f, 1.f);

		if(Stunned<StunnedChance)
		{
			SetStunned(true);
			PlayHitMontage(FName("HitReact2"));
		
		}
		else
		{
			PlayHitMontage(FName("HitReact1"));
		}
		
		const int32 RandWalkSpeedVariation = FMath::RandRange(10,150);
		const float WalkSpeedChangeChance = FMath::FRandRange(0,1.f);

		if(WalkSpeedChangeChance<=.8f)
		{
			GetCharacterMovement()->MaxWalkSpeed += RandWalkSpeedVariation;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed -= RandWalkSpeedVariation;
		}
		

		
		if(Health<=30.f && !bDead)
        	{
				if(!bCrawling)
				{
					GetCharacterMovement()->MaxWalkSpeed=FMath::RandRange(0,400);
					
				}
			if(GetCharacterMovement()->MaxWalkSpeed>=50.f)
			{
				GetCharacterMovement()->MaxWalkSpeed -= FMath::RandRange(10, 50);
			}
        		
        		bCrawling=true;
        	}
	}

	
	
		return DamageAmount;
	
}

