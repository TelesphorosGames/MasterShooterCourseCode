// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class MASTERSHOOTERCOURSE_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:
	
	AEnemyController();

	virtual void OnPossess(APawn* InPawn) override;

	FORCEINLINE UBlackboardComponent* GetMyBlackboardComponent() const {return  BlackboardComponent; }
	
	

private:
	UPROPERTY(BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	class UBlackboardComponent* BlackboardComponent;
	UPROPERTY(BlueprintReadWrite, Category="My Stuff", meta=(AllowPrivateAccess="true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;
	
};
