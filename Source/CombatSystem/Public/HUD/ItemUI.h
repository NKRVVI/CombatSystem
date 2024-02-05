// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemUI.generated.h"

class UHorizontalBox;

UCLASS()
class COMBATSYSTEM_API UItemUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetItemName(FText name);
private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemNameText;
};
