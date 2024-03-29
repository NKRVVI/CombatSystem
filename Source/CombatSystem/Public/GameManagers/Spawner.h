// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characters//BaseCharacter.h"
#include "Spawner.generated.h"

UCLASS()
class COMBATSYSTEM_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ABaseCharacter>> enemy_waves;

	UPROPERTY(EditAnywhere)
	AActor* spawn_point;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
