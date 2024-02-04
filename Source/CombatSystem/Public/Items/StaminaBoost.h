// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "StaminaBoost.generated.h"

/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API AStaminaBoost : public AItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float stamina_boost;

	virtual void GetEquipped(class ABaseCharacter* character) override;
	
};
