// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControlledCharacterOverlay.generated.h"

class UProgressBar;
class UVerticalBox;
class UHorizontalBox;
class UImage;

UCLASS()
class COMBATSYSTEM_API UControlledCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	void SetHealthPercent(float percent);
	void SetStaminaPercent(float percent);
	void FlashStaminaBar();
	void SetStaminaOpacityToFull();
	void ToggleControlsDisplay();
	void TurnOnDeathImage();
	void TurnOffDeathImage();

private:
	bool flash_down = true;
	bool flash_up = false;
	bool turn_on_death_screen;

	UPROPERTY(EditAnywhere)
	float flash_rate = 10;

	UPROPERTY(EditAnywhere)
	float death_screen_appearence_rate = 2;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* ControlsDisplay;

	UPROPERTY(meta = (BindWidget))
	UImage* DeathImage;
};