// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletHitInterface.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class MASTERSHOOTERCOURSE_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	FORCEINLINE FString GetHeadBone() const {return HeadBone ; }
	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const {return BehaviorTree; }
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation, bool bHeadShot);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	void EnemyDeath();

	void PlayHitMontage(FName Section, float PlayRate = 1.0f);
	
	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(FName Section, float PlayRate = 1.0f);

	UFUNCTION(BlueprintPure)
	FName GetAttackSectionName();
	
	UFUNCTION(BlueprintCallable)
	void StoreHitNumer(UUserWidget* HitNumber, FVector Location);

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

    UFUNCTION()
	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);

	UFUNCTION()
	void AttackRangeSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void AttackRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	class UParticleSystem* ImpactParticles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	class USoundCue* ImpactSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	float Health;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	FString HeadBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	float HealthBarDisplayTime;
	FTimerHandle HealthBarTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	UAnimMontage* HitMontage;

	// Map that stores hit number widgets and their corresponding hit locations ( FOR PROJECTING TO SCREEN )
	UPROPERTY(VisibleAnywhere, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	TMap<UUserWidget*, FVector> HitNumbers;
// How long before the hit numbers are removed from screen
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	float HitNumberDestoryTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	class UBehaviorTree* BehaviorTree;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="My Stuff", meta=(AllowPrivateAccess="true", MakeEditWidget="true"))
	FVector PatrolPoint;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="My Stuff", meta=(AllowPrivateAccess="true", MakeEditWidget="true"))
	FVector PatrolPoint2;
	UPROPERTY(VisibleAnywhere, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	class AEnemyController* EnemyController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	class USphereComponent* AgroSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	bool bStunned;
	// Percentage stun chance - 0 = no chance, 1 = 100% chance of being stunned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	float StunnedChance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	bool bInAttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	USphereComponent* AttackRangeSphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	FName Attack1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	FName Attack2;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BulletHit_Implementation(FHitResult HitResult) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
};
