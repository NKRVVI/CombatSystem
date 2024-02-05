// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ItemUI.h"
#include "Components/TextBlock.h"

void UItemUI::SetItemName(FText name)
{
	if (ItemNameText) ItemNameText->SetText(name);
}