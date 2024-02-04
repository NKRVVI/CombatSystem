// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManagers/EnemyCommander.h"
#include "Characters/Enemy.h"

// Sets default values
AEnemyCommander::AEnemyCommander()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


void AEnemyCommander::AlarmGroup()
{
	for (AEnemy* enemy : enemy_list)
	{
		if (combat_target) enemy->SetCombatTarget(combat_target);
	}
}

// Called when the game starts or when spawned
void AEnemyCommander::BeginPlay()
{
	Super::BeginPlay();

	for (AEnemy* enemy : enemy_list)
	{
		enemy->SetEnemyCommander(this);
	}
}

void AEnemyCommander::CheckForCombatTarget()
{
	AActor* target = nullptr;

	for (AEnemy* enemy : enemy_list)
	{
		
	}
}

// Called every frame
void AEnemyCommander::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

