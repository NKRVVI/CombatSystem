// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CombatSystemHUD.h"
#include "HUD/ControlledCharacterOverlay.h"

void ACombatSystemHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();

	if (world)
	{
		APlayerController* player_controller = world->GetFirstPlayerController();
		if (player_controller && controlled_character_overlay_class)
		{
			overlay = CreateWidget<UControlledCharacterOverlay>(player_controller, controlled_character_overlay_class);
			overlay->AddToViewport();
		}
	}
}