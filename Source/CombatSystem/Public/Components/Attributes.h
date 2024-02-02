// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Attributes.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBATSYSTEM_API UAttributes : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributes();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ReceiveDamage(float damage);
	void UseStamina(float stamina_cost);
	float GetHealthPercent();
	float GetStaminaPercent();
	void RegenerateStamina();
	void UseStaminaThroughSprint();
	void UseStaminaThroughRoll();
	bool CanUseStamina(float);
	float GetRollCost();
	void Respawn();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float max_health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float max_stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float sprint_cost_per_second;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float stamina_regen_per_second;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float roll_cost;

public:
	FORCEINLINE float GetHealth() { return health; }
	FORCEINLINE float GetStamina() { return stamina; }
};
