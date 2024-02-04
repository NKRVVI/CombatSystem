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

	void AlarmGroup(AActor* target);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly)
	TArray<class AEnemy*> enemy_list;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
