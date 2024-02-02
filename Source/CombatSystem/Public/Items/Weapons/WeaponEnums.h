#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	EWT_OneHandedWeapon UMETA(DisplayName = "One-Handed Weapon"),
	EWT_TwoHandedWeapon UMETA(DisplayName = "Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EWeaponDamageType : uint8 {
	EWDT_Cut UMETA(DisplayName = "Cutting Weapon"),
	EWDT_Blunt UMETA(DisplayName = "Blunt Weapon")
};