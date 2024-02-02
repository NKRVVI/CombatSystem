// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/CharacterEnums.h"
#include "ControlledCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API UControlledCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float delta_time) override;

	UPROPERTY(BlueprintReadOnly)
	class AControlledCharacter* controlled_character;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* controlled_character_movement;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	float ground_speed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FVector2D ground_velocity;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	bool is_falling;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement);
	ECharacterWeaponState character_weapon_state;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	ECharacterShieldState character_shield_state;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	EMode action_mode;
};
