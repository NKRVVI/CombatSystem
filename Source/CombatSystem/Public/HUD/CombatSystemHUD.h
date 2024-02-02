// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CombatSystemHUD.generated.h"

class UControlledCharacterOverlay;
class UControlsOverlay;

UCLASS()
class COMBATSYSTEM_API ACombatSystemHUD : public AHUD
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UControlledCharacterOverlay> controlled_character_overlay_class;

	UPROPERTY()
	UControlledCharacterOverlay* overlay;

public:
	FORCEINLINE UControlledCharacterOverlay* GetCombatSystemOverlay() const { return overlay; }
};
