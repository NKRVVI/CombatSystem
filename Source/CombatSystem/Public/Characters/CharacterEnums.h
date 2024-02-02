#pragma once

UENUM(BlueprintType)
enum class ECharacterWeaponState :uint8 {
	ESC_Unequipped UMETA(DisplayName = "Unequipped"),
	ESC_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ESC_EquippedTwoHandWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class ECharacterShieldState : uint8
{
	ECSS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECSS_EquippedShield UMETA(DisplayName = "Equipped Shield"),
	ECSS_Blocking UMETA(DisplayName = "Blocking")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Dead UMETA(DisplayName = "Dead"),
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Equipping UMETA(DisplayName = "Equipping"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_Dodging UMETA(DisplayName = "Dodging"),
	EAS_Rolling UMETA(DisplayName = "Rolling")
};

UENUM(BlueprintType)
enum class EMode : uint8
{
	EM_Standard UMETA(DisplayName = "Standard"),
	EM_Engaged UMETA(DisplayName = "Engaged"),
	EM_Sprinting UMETA(DisplayName = "Sprinting"),
	EM_RecoveringStamina UMETA(DisplayName = "Recovering Stamina")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Engaged UMETA(DisplayName = "Engaged"),
	EES_Attacking UMETA(DisplayName = "Attacking")
};