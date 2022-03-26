// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName="Damaged"),
	EIR_Common UMETA(DisplayName="Common"),
	EIR_Uncommon UMETA(DisplayName="Uncommon"),
	EIR_Rare UMETA(DisplayName="Rare"),
	EIR_Legendary UMETA(DisplayName="Legendary"),

	EIR_MAX UMETA(DisplayName="DefaultMAX")
};


UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_OnGround UMETA(DisplayName="OnGround"),
	EIS_EquipInterping UMETA(DisplayName="EquipInterping"),
	EIS_Equipped UMETA(DisplayName="Equipped"),
	EIS_Falling UMETA(DisplayName="Falling"),
	EIS_PickedUp UMETA(DisplayName="PickedUp"),

	EIS_MAX UMETA(DisplayName="DefaultMAX")
};

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

	void SetActiveStars();

	// Sets properties of the Item's compents based on state
	void SetItemProperties(EItemState State);

	//Called when the intem interp timer is finished, E.G. Item is ready to be picked up / consumed
	void FinishInterping();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE class UWidgetComponent* GetPickupWidget() const { return PickupWidget ; }
	FORCEINLINE class USphereComponent* GetAreaSphere() const { return AreaSphere ;}
	FORCEINLINE class UBoxComponent* GetCollisionBox() const { return CollisionBox ;}
	FORCEINLINE EItemState GetItemState() const { return ItemState ;}
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const {return ItemMesh ;}

	void SetItemState(EItemState State);

	// Called From the character class when equipping items
	void StartItemCurve(AMainCharacter* Character);

	
	
private:

	// Line traces will collide with item collision boxes to generate HUD elements, etc
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	UBoxComponent* CollisionBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	USkeletalMeshComponent* ItemMesh;


	//Popup widget for when player looks at item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	UWidgetComponent* PickupWidget;


	// Enables Item tracing during overlap
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	int32 ItemCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	EItemRarity ItemRarity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	TArray<bool> ActiveStars;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	EItemState ItemState;

	//Curve asset used for Item's interpolation to camera
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	class UCurveFloat* ItemZCurve;
	
	// Starting location for intem when interping begins
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	FVector ItemInterpStartLocation={FVector(0.f)}; 

	// Target location for item to interp to in front of camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	FVector ItemInterpTargetLocation={FVector(0.f)};

	// Set to true while interping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	bool bInterping = false;

	//Timer that runs when interping begins
	FTimerHandle ItemPickupInterpTimer;

	// Pointer to the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	 AMainCharacter* CharacterPointer;

	/* Duration of Curve & timer - This is set to the same time length as the curve we created
	 * in Unreal engine - they match so that after the curve has finished, the callback function
	 * will be called to dictate what to do with the item. 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	float ZCurveInterpTime = 0.7f;

	//Handles the actual movement of the item when it is in EquipInterping state
	void ItemInterp(float DeltaTime);

	float ItemInterpX = 0.f ;
	float ItemInterpY = 0.f ;


	//Initial Yaw Offset between camera and item, used to ensure item rotation faces camera
	// Used to match the camera's Yaw direction
	float InterpInitialYawOffset = 0.f ;
	
};

