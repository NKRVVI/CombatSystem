// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy.h"
#include "Animation/AnimMontage.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Items/Weapons/Weapon.h"
#include "HUD/HealthBarComponent.h"
#include "Components/Attributes.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	pawn_sensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	pawn_sensing->HearingThreshold = 500.f;
	pawn_sensing->SightRadius = 4000.f;
	pawn_sensing->SetPeripheralVisionAngle(45.f);

	health_bar_widget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	health_bar_widget->SetupAttachment(GetRootComponent());

}

void AEnemy::GetHit(AActor* hitter, AWeapon* weapon, FVector impact_point)
{
	Super::GetHit(hitter, weapon, impact_point);

	if(IsAlive())
	{
		ClearPatrolTimer();
		//UE_LOG(LogTemp, Warning, TEXT("[GetHit] ClearAttackTimer"));
		ClearAttackTimer();
		combat_target = hitter;
		StopAttackMontage();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (!IsAlive())
	{
		enemy_state = EEnemyState::EES_Dead;
	}
	else if (enemy_state < EEnemyState::EES_Chasing)
	{
		enemy_state = EEnemyState::EES_Chasing;
	}
	return DamageAmount;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	UpdateHealthHUD();

	if (pawn_sensing) pawn_sensing->OnSeePawn.AddDynamic(this, &AEnemy::OnPawnSeen);
	enemy_controller = Cast<AAIController>(GetController());
	MoveToTarget(current_patrol_target);
	GetCharacterMovement()->MaxWalkSpeed = patrolling_speed;
	Tags.Add(FName("Enemy"));
	SpawnDefaultWeapon();
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> rest_of_patrol_targets;
	for (AActor* actor : patrol_targets)
	{
		if (actor != current_patrol_target) rest_of_patrol_targets.Add(actor);
	}

	if (rest_of_patrol_targets.Num() > 0)
	{
		return rest_of_patrol_targets[FMath::RandRange(0, rest_of_patrol_targets.Num() - 1)];
	}

	return nullptr;
}

void AEnemy::StartPatrolTimer()
{
	GetWorldTimerManager().SetTimer(patrol_timer, this, &AEnemy::PatrolTimerFinished, FMath::RandRange(patrol_wait_time_min, patrol_wait_time_max));
}

void AEnemy::StartAttackTimer()
{
	enemy_state = EEnemyState::EES_Engaged;
	float attack_delay = FMath::RandRange(attack_wait_time_min, attack_wait_time_max);
	GetWorldTimerManager().SetTimer(attack_timer, this, &AEnemy::Attack, attack_delay);
}

void AEnemy::Attack(const FInputActionValue& value)
{
	Super::Attack(value);

	if (!combat_target) return;
	equipped_weapon->EnableGhostWeaponBoxCollision();
	enemy_state = EEnemyState::EES_Attacking;
	if (static_cast<int>(equipped_weapon->GetWeaponType()) < attack_montages.Num()) PlayRandomMontageSection(attack_montages[static_cast<int>(equipped_weapon->GetWeaponType())]);
	enemy_controller->StopMovement();
}

void AEnemy::Attack()
{
	Attack(FInputActionValue());
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(patrol_timer);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(attack_timer);
}

void AEnemy::HitReactEnd()
{
	if (!combat_target) return;

	if (WithinDistanceFromTarget(combat_target, attack_radius))
	{
		//UE_LOG(LogTemp, Warning, TEXT("[HitReactEnd] Attack Timer started"));
		StartAttackTimer();
	}
	else if (WithinDistanceFromTarget(combat_target, chase_radius))
	{
		ChaseCombatTarget();
	}
	else
	{
		enemy_state = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = patrolling_speed;
		MoveToTarget(current_patrol_target);
	}
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* world = GetWorld();

	if (world && weapon_class)
	{
		AWeapon* spawned_weapon = world->SpawnActor<AWeapon>(weapon_class);
		spawned_weapon->GetEquipped(this);
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (WithinDistanceFromTarget(current_patrol_target, patrol_radius))
	{
		current_patrol_target = ChoosePatrolTarget();
		if (current_patrol_target) StartPatrolTimer();
	}
}

void AEnemy::AttackEnd()
{
	if (!combat_target) return;

	if (WithinDistanceFromTarget(combat_target, attack_radius))
	{
		//UE_LOG(LogTemp, Warning, TEXT("[Attack End] Attack timer started"));
		StartAttackTimer();
	}
	else if (WithinDistanceFromTarget(combat_target, chase_radius))
	{
		ChaseCombatTarget();
	}
}

void AEnemy::OnPawnSeen(APawn* seen_pawn)
{
	if (enemy_state > EEnemyState::EES_Patrolling || IsDead()) return;
	combat_target = seen_pawn;
	ClearPatrolTimer();
	enemy_state = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = chasing_speed;
	MoveToTarget(combat_target);
}

void AEnemy::MoveToTarget(AActor* target)
{
	if (enemy_controller == nullptr || target == nullptr) return;

	FAIMoveRequest move_request;
	move_request.SetGoalActor(target);
	move_request.SetAcceptanceRadius(50.f);
	enemy_controller->MoveTo(move_request);
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(current_patrol_target);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDead()) return;
	if (enemy_state > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::CheckCombatTarget()
{
	if (!WithinDistanceFromTarget(combat_target, chase_radius))
	{
		//UE_LOG(LogTemp, Warning, TEXT("[CheckCombatTarget] ClearAttackTimer"));
		ClearAttackTimer();
		enemy_state = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = patrolling_speed;
		MoveToTarget(current_patrol_target);
		combat_target = nullptr;
	}
	else if (!WithinDistanceFromTarget(combat_target, attack_radius) && enemy_state != EEnemyState::EES_Attacking)
	{
		if (enemy_state != EEnemyState::EES_Chasing) enemy_state = EEnemyState::EES_Chasing;
		//UE_LOG(LogTemp, Warning, TEXT("[CheckCombatTarget] ClearAttackTimer"));
		ClearAttackTimer();
		MoveToTarget(combat_target);
		GetCharacterMovement()->MaxWalkSpeed = chasing_speed;
	}
	else if (enemy_state < EEnemyState::EES_Engaged)
	{
		//UE_LOG(LogTemp, Warning, TEXT("[CheckCombatTarget] Attack Timer started"));
		StartAttackTimer();
	}
}

void AEnemy::ChaseCombatTarget()
{
	enemy_state = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = chasing_speed;
	MoveToTarget(combat_target);
}

void AEnemy::Die_Implementation(FVector impact_point)
{
	Super::Die_Implementation(impact_point);

	SetLifeSpan(death_life_span);
	if (equipped_weapon && !equipped_weapon->IsPickupable()) equipped_weapon->SetLifeSpan(death_life_span);
	HideHealthBar();
	enemy_state = EEnemyState::EES_Dead;
	ClearPatrolTimer();
	ClearAttackTimer();
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECR_Block);
}

void AEnemy::HideHealthBar()
{
	if (!health_bar_widget) return;
	health_bar_widget->SetVisibility(false);
}

void AEnemy::UpdateHealthHUD()
{
	if (health_bar_widget && attributes) health_bar_widget->SetHealthPercent(attributes->GetHealthPercent());
}
