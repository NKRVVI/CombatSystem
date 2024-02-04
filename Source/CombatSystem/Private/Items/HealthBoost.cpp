// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/HealthBoost.h"
#include "Characters/BaseCharacter.h"
#include "Characters/ControlledCharacter.h"

void AHealthBoost::GetEquipped(ABaseCharacter* character)
{
	if (AControlledCharacter* controlled_character = Cast<AControlledCharacter>(character))
	{
		controlled_character->GetHealthBoost(health_boost);
		Destroy();
	}
}
