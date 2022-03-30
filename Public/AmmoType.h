#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_Pistol UMETA(DisplayName = "PistolAmmo"),
	EAT_AssaultRifle UMETA(DisplayName = "AssaultRifleAmmo"),
	EAT_LongRangeRifle UMETA(DisplayName = "LongRangeRifle"),

	
	EAT_MAX UMETA(DisplayName = "DefaultMax"),
	
};