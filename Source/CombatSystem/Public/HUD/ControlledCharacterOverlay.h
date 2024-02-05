// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControlledCharacterOverlay.generated.h"

class UProgressBar;
class UVerticalBox;
class UHorizontalBox;
class UImage;
class UTextBlock;

UCLASS()
class COMBATSYSTEM_API UControlledCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	void SetHealthPercent(float percent);
	void SetShadowHealthPercent(float percent);
	void SetStaminaPercent(float percent);
	void FlashStaminaBar();
	void SetStaminaOpacityToFull();
	void ToggleControlsDisplay();
	void TurnOnDeathImage();
	void TurnOffDeathImage();
	void ShowHealthIncrement(float increment);
	void ShowStaminaIncrement(float increment);
	void FollowHealthBar();
	void FollowShadowHealthBar();
	void SetShadowHealthBarToHealthBar();

protected:
	FTimerHandle shadow_health_bar_follow_timer;

	UPROPERTY(EditAnywhere)
	float shadow_health_bar_follow_delay = 0.5f;

	void StartFollowHealthBar();
	void StartFollowShadowHealthBar();
private:
	bool flash_down = true;
	bool flash_up = false;
	bool turn_on_death_screen;
	bool is_health_incremented = false;
	bool is_stamina_incremented = false;
	bool shall_follow_health_bar = false;
	bool shall_follow_shadow_health_bar = false;
	bool shall_follow_stamina_bar = false;

	UPROPERTY(EditAnywhere)
	float flash_rate = 10;

	UPROPERTY(EditAnywhere)
	float death_screen_appearence_rate = 2;

	UPROPERTY(EditAnywhere)
	float health_stamina_increment_disappearance_rate = 3;

	UPROPERTY(EditAnywhere)
	float follow_bar_interp_rate = 1;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShadowHealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShadowStaminaBar;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* ControlsDisplay;

	UPROPERTY(meta = (BindWidget))
	UImage* DeathImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthIncrementText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StaminaIncrementText;
};