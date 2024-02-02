// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ControlledCharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"

void UControlledCharacterOverlay::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	if (turn_on_death_screen)
	{
		float target_opacity = FMath::FInterpTo(DeathImage->GetRenderOpacity(), 1, GetWorld()->GetDeltaSeconds(), death_screen_appearence_rate);
		DeathImage->SetRenderOpacity(target_opacity);
	}
}

void UControlledCharacterOverlay::SetHealthPercent(float percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(percent);
	}
}

void UControlledCharacterOverlay::SetStaminaPercent(float percent)
{
	if (StaminaBar)
	{
		StaminaBar->SetPercent(percent);
	}
}

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
