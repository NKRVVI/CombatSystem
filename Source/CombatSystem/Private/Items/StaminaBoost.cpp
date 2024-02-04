// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/StaminaBoost.h"
#include "Characters/BaseCharacter.h"
#include "Characters/ControlledCharacter.h"

void AStaminaBoost::GetEquipped(ABaseCharacter* character)
{
	if (AControlledCharacter* controlled_character = Cast<AControlledCharacter>(character))
	{
		controlled_character->GetStaminaBoost(stamina_boost);
		Destroy();
	}
}
