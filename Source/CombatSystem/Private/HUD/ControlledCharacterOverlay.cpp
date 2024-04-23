// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ControlledCharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"


void UControlledCharacterOverlay::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	//if character is dead, then death screen is turned on
	if (turn_on_death_screen)
	{
		float target_opacity = FMath::FInterpTo(DeathImage->GetRenderOpacity(), 1, GetWorld()->GetDeltaSeconds(), death_screen_appearence_rate);
		DeathImage->SetRenderOpacity(target_opacity);
	}
	
	// setting the health increment text
	if (is_health_incremented)
	{
		float target_opacity = FMath::FInterpTo(HealthIncrementText->GetRenderOpacity(), 0, GetWorld()->GetDeltaSeconds(), health_stamina_increment_disappearance_rate);
		HealthIncrementText->SetRenderOpacity(target_opacity);
		if (FMath::IsNearlyZero(target_opacity)) is_health_incremented = false;
	}

	// setting the stamina increment text
	if (is_stamina_incremented)
	{
		float target_opacity = FMath::FInterpTo(StaminaIncrementText->GetRenderOpacity(), 0, GetWorld()->GetDeltaSeconds(), health_stamina_increment_disappearance_rate);
		StaminaIncrementText->SetRenderOpacity(target_opacity);

		if (FMath::IsNearlyZero(target_opacity)) is_stamina_incremented = false;
	}

	// setting the following movement between the shadow health bar and the health bar
	if (shall_follow_health_bar)
	{
		float target_percentage = FMath::FInterpConstantTo(ShadowHealthBar->Percent, HealthBar->Percent, GetWorld()->GetDeltaSeconds(), follow_bar_interp_rate);
		ShadowHealthBar->SetPercent(target_percentage);

		if (FMath::IsNearlyEqual(target_percentage, HealthBar->Percent)) shall_follow_health_bar = false;
	}
	else if(shall_follow_shadow_health_bar)
	{
		float target_percentage = FMath::FInterpConstantTo(HealthBar->Percent, ShadowHealthBar->Percent, GetWorld()->GetDeltaSeconds(), follow_bar_interp_rate);
		HealthBar->SetPercent(target_percentage);

		if (FMath::IsNearlyEqual(target_percentage, ShadowHealthBar->Percent)) shall_follow_shadow_health_bar = false;
	}
}

void UControlledCharacterOverlay::SetHealthPercent(float percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(percent);
	}
}

void UControlledCharacterOverlay::SetShadowHealthPercent(float percent)
{
	if (ShadowHealthBar) ShadowHealthBar->SetPercent(percent);
}

void UControlledCharacterOverlay::SetStaminaPercent(float percent)
{
	if (StaminaBar)
	{
		StaminaBar->SetPercent(percent);
	}
}

// flashing stamina bar when recovering in health
void UControlledCharacterOverlay::FlashStaminaBar()
{
	if (flash_down)
	{
		float target_opacity = FMath::FInterpTo(StaminaBar->GetRenderOpacity(), 0, GetWorld()->GetDeltaSeconds(), flash_rate);
		StaminaBar->SetRenderOpacity(target_opacity);
		if (target_opacity == 0)
		{
			flash_down = false;
			flash_up = true;
		}
	}
	else
	{
		float target_opacity = FMath::FInterpTo(StaminaBar->GetRenderOpacity(), 1, GetWorld()->GetDeltaSeconds(), flash_rate);
		StaminaBar->SetRenderOpacity(target_opacity);
		if (target_opacity == 1)
		{
			flash_up = false;
			flash_down = true;
		}
	}
}

// used when shifting from recovering stamina mode to standard, opacity of stamina bar is reset
void UControlledCharacterOverlay::SetStaminaOpacityToFull()
{
	if (StaminaBar) StaminaBar->SetRenderOpacity(1.f);
}

void UControlledCharacterOverlay::ToggleControlsDisplay()
{
	if (ControlsDisplay->GetRenderOpacity() == 0)
	{
		ControlsDisplay->SetRenderOpacity(1);
	}
	else 
	{
		ControlsDisplay->SetRenderOpacity(0);
	}
}

void UControlledCharacterOverlay::TurnOnDeathImage()
{
	turn_on_death_screen = true;
	//if (DeathImage) DeathImage->SetRenderOpacity(1);
}

void UControlledCharacterOverlay::TurnOffDeathImage()
{
	if (DeathImage) DeathImage->SetRenderOpacity(0);
}

void UControlledCharacterOverlay::ShowHealthIncrement(float increment)
{
	
	if (HealthIncrementText) {
		FString prefix((increment >= 0) ? "+" : "");
		HealthIncrementText->SetText(FText::FromString(prefix + FString::SanitizeFloat(increment)));
		HealthIncrementText->SetRenderOpacity(1);
		is_health_incremented = true;
	}
}

void UControlledCharacterOverlay::ShowStaminaIncrement(float increment)
{
	if (StaminaIncrementText)
	{
		FString prefix((increment >= 0) ? "+" : "");
		StaminaIncrementText->SetText(FText::FromString(prefix + FString::SanitizeFloat(increment)));
		StaminaIncrementText->SetRenderOpacity(1);
		is_stamina_incremented = true;
	}
}

void UControlledCharacterOverlay::FollowHealthBar()
{
	GetWorld()->GetTimerManager().SetTimer(shadow_health_bar_follow_timer, this, &UControlledCharacterOverlay::StartFollowHealthBar, shadow_health_bar_follow_delay);
}

void UControlledCharacterOverlay::FollowShadowHealthBar()
{
	GetWorld()->GetTimerManager().SetTimer(shadow_health_bar_follow_timer, this, &UControlledCharacterOverlay::StartFollowShadowHealthBar, shadow_health_bar_follow_delay);
}

void UControlledCharacterOverlay::SetShadowHealthBarToHealthBar()
{
	if (ShadowHealthBar && HealthBar) ShadowHealthBar->SetPercent(HealthBar->Percent);
}

void UControlledCharacterOverlay::StartFollowHealthBar()
{
	shall_follow_health_bar = true;
	shall_follow_shadow_health_bar = false;
}

void UControlledCharacterOverlay::StartFollowShadowHealthBar()
{
	shall_follow_shadow_health_bar = true;
	shall_follow_health_bar = false;
}
