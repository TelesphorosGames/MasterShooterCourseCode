// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class MASTERSHOOTERCOURSE_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	/* 
		Create the functions used to bind the ability to detect an overlap (and overlap end) to our Item's sphere component (AreaSphere).
		
		**These function signatures are specific and must be exact when using OnComponentOverlapBegin.AddDynamic()
		(and its counterpart, OnComponentEndOverlap.AddDynamic()**

	 */
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* See OnSphereEndOverlap  */
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE bool bItemInRangeForHUD() const { return bHudDisplayPossible; }

private:

	// Line traces will collide with item collision boxes to generate HUD elements, etc
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	class UBoxComponent* CollisionBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	USkeletalMeshComponent* ItemMesh;


	//Popup widget for when player looks at item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	class UWidgetComponent* PickupWidget;


	// Enables Item tracing during overlap
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	class USphereComponent* AreaSphere;
	
	bool bHudDisplayPossible; 
	
	
	
public:

	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget ; }
	
};
