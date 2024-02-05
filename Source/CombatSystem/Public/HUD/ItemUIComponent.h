// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ItemUIComponent.generated.h"

class UItemUI;

UCLASS()
class COMBATSYSTEM_API UItemUIComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetNameText(FText name);
private:
	UPROPERTY()
	UItemUI* item_ui;
};
