// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "CharacterEnums.h"
#include "InputActionValue.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UPawnSensingComponent;
class AEnemyCommander;

UCLASS()
class COMBATSYSTEM_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	virtual void GetHit(AActor* hitter, AWeapon* weapon, FVector impact_point) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AActor* ChoosePatrolTarget();
	void StartPatrolTimer();
	void StartAttackTimer();
	virtual void Attack(const FInputActionValue&) override;
	void Attack();
	void ClearPatrolTimer();
	void ClearAttackTimer();
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();
	void SpawnDefaultWeapon();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void ChaseCombatTarget();
	void Die_Implementation(FVector) override;
	void HideHealthBar();
	virtual void UpdateHealthHUD() override;
	void AlarmCommander();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditAnywhere, Category = "Patrolling")
	AActor* current_patrol_target;

	UPROPERTY(EditAnywhere, Category = "Patrolling")
	TArray<AActor*> patrol_targets;

	UPROPERTY()
	class AAIController* enemy_controller;

	UPROPERTY(VisibleAnywhere)
	EEnemyState enemy_state = EEnemyState::EES_Patrolling;

	UFUNCTION()
	void OnPawnSeen(APawn* seen_pawn);

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* pawn_sensing;

	UFUNCTION(BlueprintCallable)
	void AlarmEnd();

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* alarm_montage;

	UPROPERTY(EditDefaultsOnly)
	float entry_into_attack_radius = attack_radius;

	UPROPERTY(EditDefaultsOnly)
	float exit_from_attack_radius = attack_radius * 1.5;
private:

	void MoveToTarget(AActor* target);
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category = "Patrolling")
	double patrol_radius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Chasing")
	double chase_radius = 1000.f;

	FTimerHandle patrol_timer;
	FTimerHandle attack_timer;

	UPROPERTY(EditAnywhere, Category = "Patrolling")
	float patrol_wait_time_min = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Patrolling")
	float patrol_wait_time_max = 10.f;

	UPROPERTY(EditAnywhere, Category = "Patrolling")
	float patrolling_speed = 125.f;

	UPROPERTY(EditAnywhere, Category = "Chasing")
	float chasing_speed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Attacking")
	float attack_wait_time_min = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Attacking")
	float attack_wait_time_max = 1.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> weapon_class;

	UPROPERTY(VisibleAnywhere)
	class UHealthBarComponent* health_bar_widget;

	UPROPERTY(EditAnywhere, Category = Combat)
	float death_life_span = 8.f;

	UPROPERTY(EditInstanceOnly)
	AEnemyCommander* enemy_commander;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EEnemyState GetEnemyState() { return enemy_state; }

	UFUNCTION(BlueprintCallable)
	void SetEnemyState(EEnemyState state) { enemy_state = state; }

	void SetCombatTarget(AActor* target);

	void SetEnemyCommander(AEnemyCommander* commander);
};
