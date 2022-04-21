// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"



#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Common UMETA(DisplayName="Common"),
	EIR_Great UMETA(DisplayName="Great"),
	EIR_Rare UMETA(DisplayName="Rare"),
	EIR_Superb UMETA(DisplayName="Superb"),
	EIR_Epic UMETA(DisplayName="Epic"),

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

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Ammo UMETA(DisplayName="Ammo"),
	EIT_Weapon UMETA(DisplayName="Weapon"),

	EIT_MAX UMETA(DisplayName="DefaultMAX")
};

USTRUCT(BlueprintType)
struct FItemRarityTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor GlowColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LightColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DarkColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfStars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* IconBackground;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CustomDepthStencil;
	
};

UCLASS()
class MASTERSHOOTERCOURSE_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	 
	// Sets default values for this actor's properties
	AItem();
	virtual void Tick(float DeltaTime) override;

	
	FORCEINLINE class UWidgetComponent* GetPickupWidget() const { return PickupWidget ; }
	FORCEINLINE class USphereComponent* GetAreaSphere() const { return AreaSphere ;}
	FORCEINLINE class UBoxComponent* GetCollisionBox() const { return CollisionBox ;}
	FORCEINLINE EItemState GetItemState() const { return ItemState ;}
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const {return ItemMesh ;}
	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound ;}
	FORCEINLINE USoundCue* GetEquipSound() const {return EquipSound ;}
	FORCEINLINE int32 GetItemCount() const { return ItemCount ; }
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex ; }
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index ; }
	FORCEINLINE void SetCharacterInventoryFull(bool Full) { bCharacterInventoryFull = Full ; }


	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/* 
		Create the functions used to bind the ability to detect an overlap
		(and overlap end) to our Item's sphere component (AreaSphere).
		** These function signatures are specific and must be exact
		* when using OnComponentOverlapBegin.AddDynamic()
		(and its counterpart, OnComponentEndOverlap.AddDynamic() ) **
	 */
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	/* See OnSphereEndOverlap  */
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// Used to determine how many stars should appear on pickup widget, based on ItemRarity
	void SetActiveStars();
	// Sets properties of the Item's compents based on state
	virtual void SetItemProperties(EItemState State);
	//Called when the intem interp timer is finished, E.G. Item is ready to be picked up / consumed
	void FinishInterping();

	//Get Interp Location based on item type and spots available
	FVector GetInterpLocation();
	void ItemInterp(float DeltaTime);
	
	virtual void InitializeCustomDepth();

	virtual void OnConstruction(const FTransform& Transform) override;


public:
	void EnableGlowMaterial();
	void DisableGlowMaterial();
	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();
	// Used to determine what the item is doing and how we can interact with it.
	void SetItemState(EItemState State);
	// Called From the character class when equipping items
	void StartItemCurve(AMainCharacter* Character);
	// Set to true while interping
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
    bool bInterping = false;

	
	void UpdatePulseEffect();

	void ResetPulseTimer();
	
	void StartPulseTimer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Item Properties")
	bool bIsOverlappingChar =false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Item Properties")
	int32 DistanceToCharacter;
    	
private:

	// Line traces will collide with item collision boxes to generate HUD elements, etc
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	UBoxComponent* CollisionBox;
	//Popup widget for when player looks at item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	UWidgetComponent* PickupWidget;
	// Enables Item tracing during overlap
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	USphereComponent* AreaSphere;
	// The SKELETAL mesh component ( if any ) that the item has - (used for weapons, etc. )
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	USkeletalMeshComponent* ItemMesh;
	//The name of the Item ( instance editable )
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	FString ItemName;
	// How many of the item are included in the pickup? (ammo will have an amount)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	int32 ItemCount;
	// Will drive how many stars appear on the item's pickup widget - more rarity should be a better item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	EItemRarity ItemRarity;
	// Array that holds the info for how many stars should appear according to ItemRarity
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	TArray<bool> ActiveStars;
	// What is the item doing? Used to determine if we can pick up / use item
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

	//Timer that runs when interping begins
	FTimerHandle ItemPickupInterpTimer;
	// Pointer to the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	 AMainCharacter* CharacterPointer;
	/* Duration of Curve & timer - This is set to the same time length as the curve we created
	 in Unreal engine - they match so that after the curve has finished,
	 the callback function will be called to dictate what to do with the item. 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	float ZCurveInterpTime;
	//Handles the actual movement of the item when it is in EquipInterping state

	float ItemInterpX = 0.f ;
	float ItemInterpY = 0.f ;
	//Initial Yaw Offset between camera and item, used to ensure item rotation faces camera
	//- Used to match the camera's Yaw direction
	// float InterpInitialYawOffset = 0.f ;
	// Responsible for scaling the item down to 0 when we pick it up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	UCurveFloat* ItemScaleCurve;
	// Sound that plays when we pick the item up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	USoundCue* PickupSound;
	// Sound that plays when we equip the item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	EItemType ItemType;

	// Index of InterpLocation this item is interping to
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	int32 InterpLocIndex;

	// Index of the location we'd like to change at runtime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	int32 MaterialIndex;

	// Dynamic instance that can be changed at runtime
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	// Material instance to be used with DynamicMaterialInstance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	UMaterialInstance* MaterialInstance;
	// Curve to drive the dynamic material parameters - makes our gun's flash pulse effect

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	UCurveVector* PulseCurve;
    
	FTimerHandle PulseTimer;
    	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	float PulseCurveTime;
    
	// Linked to our PulseCurve and our GlowAmount parameter on our flashy lines material
	UPROPERTY(VisibleAnywhere,Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	float GlowAmount;
    
	UPROPERTY(VisibleAnywhere,Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	float FresnelExponent;
    
	UPROPERTY(VisibleAnywhere,Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	float FresnelReflectFraction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Item Properties", meta = (AllowPrivateAccess="true"))
	UCurveVector* MaterialInterpPulseCurve;
	
	// Icon for this item shown in inventory hud widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="My Stuff | Inventory", meta = (AllowPrivateAccess="true"))
	UTexture2D* IconItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="My Stuff | Inventory", meta = (AllowPrivateAccess="true"))
	UTexture2D* AmmoItem;
	// Slot in the inventory array
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Inventory", meta = (AllowPrivateAccess="true"))
	int32 SlotIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="My Stuff | Inventory", meta = (AllowPrivateAccess="true"))
	bool bCharacterInventoryFull;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="My Stuff | Data Tables", meta=(AllowPrivateAccess=true))
	UDataTable* ItemRarityDataTable;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="My Stuff | Data Tables", meta=(AllowPrivateAccess=true))
	FLinearColor RowGlowColor;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="My Stuff | Data Tables", meta=(AllowPrivateAccess=true))
	FLinearColor RowLightColor;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="My Stuff | Data Tables", meta=(AllowPrivateAccess=true))
	FLinearColor RowDarkColor;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="My Stuff | Data Tables", meta=(AllowPrivateAccess=true))
	int32 RowNumberOfStars;
	//Background color image for this item, shown in the inventory hud widget
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="My Stuff | Data Tables", meta=(AllowPrivateAccess=true))
	class UTexture2D* IconBackground;
	
	
};


