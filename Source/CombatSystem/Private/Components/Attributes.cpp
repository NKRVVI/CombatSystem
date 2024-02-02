// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Attributes.h"

// Sets default values for this component's properties
UAttributes::UAttributes()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttributes::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAttributes::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAttributes::ReceiveDamage(float damage)
{
	health = FMath::Clamp(health - damage, 0.f, max_health);
}

void UAttributes::UseStamina(float stamina_cost)
{
	stamina = FMath::Clamp(stamina - stamina_cost, 0.f, max_stamina);
}

float UAttributes::GetHealthPercent()
{
	return health / max_health;
}

float UAttributes::GetStaminaPercent()
{
	return stamina / max_stamina;
}

void UAttributes::RegenerateStamina()
{
	if (UWorld* world = GetWorld())
	{
		stamina = FMath::Clamp(stamina + stamina_regen_per_second * world->GetDeltaSeconds(), 0.f, max_stamina);
	}
}

void UAttributes::UseStaminaThroughSprint()
{
	if (UWorld* world = GetWorld())
	{
		UseStamina(sprint_cost_per_second * world->GetDeltaSeconds());
	}
}

void UAttributes::UseStaminaThroughRoll()
{
	UseStamina(roll_cost);
}

bool UAttributes::CanUseStamina(float cost)
{
	return GetStamina() >= cost;
}

float UAttributes::GetRollCost()
{
	return roll_cost;
}

void UAttributes::Respawn()
{
	health = max_health;
	stamina = max_stamina;
}
