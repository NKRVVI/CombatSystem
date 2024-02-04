// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "HealthBoost.generated.h"

/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API AHealthBoost : public AItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float health_boost = 0;

	virtual void GetEquipped(class ABaseCharacter* character);
};
