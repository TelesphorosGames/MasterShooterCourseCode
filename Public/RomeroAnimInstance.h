// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RomeroAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MASTERSHOOTERCOURSE_API URomeroAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);


private:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	float Speed;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	class AEnemy* Enemy;
};
