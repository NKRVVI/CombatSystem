// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ItemUIComponent.h"
#include "HUD/ItemUI.h"

void UItemUIComponent::SetNameText(FText name)
{
	if (!item_ui) item_ui = Cast<UItemUI>(GetUserWidgetObject());
	
	if(item_ui) item_ui->SetItemName(name);

}
