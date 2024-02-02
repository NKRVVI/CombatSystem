// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyAnimInstance.h"
#include "Characters/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	enemy = Cast<AEnemy>(TryGetPawnOwner());
	if (enemy)
	{
		enemy_character_movement = enemy->GetCharacterMovement();
	}
}

void UEnemyAnimInstance::NativeUpdateAnimation(float delta_time)
{
	Super::NativeUpdateAnimation(delta_time);

	if (enemy_character_movement)
	{
		ground_speed = UKismetMathLibrary::VSizeXY(enemy_character_movement->Velocity);
		enemy_state = enemy->GetEnemyState();
	}
}
