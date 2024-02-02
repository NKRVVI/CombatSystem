// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Interfaces/HitInterface.h"
#include "Shield.generated.h"

/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API AShield : public AItem
{
	GENERATED_BODY()
public:
	void GetEquipped(class ABaseCharacter* actor) override;
	AShield();
	virtual void BeginPlay() override;

protected:

	UFUNCTION()
	void OnShieldBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere, Category = "Shield Properties")
	class UBoxComponent* shield_box;

public:
	void DisableShieldBoxCollision();
	void EnableShieldBoxCollision();
};
