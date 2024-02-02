// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float percent)
{
	if (health_bar_widget == nullptr) health_bar_widget = Cast<UHealthBar>(GetUserWidgetObject());

	if (health_bar_widget && health_bar_widget->HealthBar)
	{
		health_bar_widget->HealthBar->SetPercent(percent);
	}
}
