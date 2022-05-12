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

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BulletHit_Implementation(FHitResult HitResult) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
};
