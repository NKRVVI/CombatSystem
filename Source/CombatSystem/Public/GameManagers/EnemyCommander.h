// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyCommander.generated.h"

UCLASS()
class COMBATSYSTEM_API AEnemyCommander : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyCommander();

	void AlarmGroup();
	bool IsAlreadyInformed() { if (combat_target) return true; else return false; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void CheckForCombatTarget();

	UPROPERTY(EditInstanceOnly)
	TArray<class AEnemy*> enemy_list;

	AActor* combat_target;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetCombatTarget(AActor* target) { combat_target = target; }
};
