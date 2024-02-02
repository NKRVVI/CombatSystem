// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ControlledCharacterAnimInstance.h"
#include "Characters/ControlledCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UControlledCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	controlled_character = Cast<AControlledCharacter>(TryGetPawnOwner());
	if (controlled_character)
	{
		controlled_character_movement = controlled_character->GetCharacterMovement();
	}
}

void UControlledCharacterAnimInstance::NativeUpdateAnimation(float delta_time)
{
	Super::NativeUpdateAnimation(delta_time);

	if (controlled_character_movement)
	{
		ground_speed = UKismetMathLibrary::VSizeXY(controlled_character_movement->Velocity);
		is_falling = controlled_character_movement->IsFalling();
		character_weapon_state = controlled_character->GetCharacterWeaponState();
		character_shield_state = controlled_character->GetCharacterShieldState();
		action_mode = controlled_character->GetActionMode();
		ground_velocity = FVector2D(controlled_character->GetActorRotation().UnrotateVector(controlled_character->GetVelocity()));
	}
}