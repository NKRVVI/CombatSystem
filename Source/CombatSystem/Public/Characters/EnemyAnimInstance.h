// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/CharacterEnums.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float delta_time) override;

	UPROPERTY(BlueprintReadOnly)
	class AEnemy* enemy;

	UPROPERTY(BlueprintReadOnly)
	class UCharacterMovementComponent* enemy_character_movement;

	UPROPERTY(BlueprintReadOnly)
	float ground_speed;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState enemy_state;
};
