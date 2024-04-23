// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Animation/AnimMontage.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/Attributes.h"
#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	motion_warping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	attributes = CreateDefaultSubobject<UAttributes>(TEXT("Attributes"));
}

/*
	function overriden from the IHitInterface class. Handles getting hit by a weapon
*/
void ABaseCharacter::GetHit(AActor* hitter, AWeapon* weapon, FVector impact_point)
{
	//if (GEngine) GEngine->AddOnScreenDebugMessage(1, 10, FColor::Black, FString("Get hit"));
	UE_LOG(LogTemp, Warning, TEXT("%f"), GetHealth());
	if(IsAlive())
	{
		PlayHitReactMontage(hitter->GetActorLocation());
		SetWeaponBoxCollision(ECollisionEnabled::NoCollision);
	}
	else
	{
		Die(hitter->GetActorLocation());
	}
	PlayHitSound(impact_point, weapon->GetWeaponDamageType());
	SpawnHitParticles(impact_point);
	PlayPainSound();
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
//function to play a section from an animation montage
void ABaseCharacter::PlayMontageSection(UAnimMontage* montage, FName section)
{
	UAnimInstance* anim_instance = GetMesh()->GetAnimInstance();
	if (anim_instance && montage) {
		anim_instance->Montage_Play(montage);
		anim_instance->Montage_JumpToSection(section, montage);
	}
}
 // function to play a random section from a montage
void ABaseCharacter::PlayRandomMontageSection(UAnimMontage* montage)
{
	if (!montage) return;
	int32 num_sections = montage->CompositeSections.Num();
	int32 random_selection = FMath::RandRange(0, num_sections - 1);
	FName random_section_name = montage->GetSectionName(random_selection);

	PlayMontageSection(montage, random_section_name);
}

// plays the appropriate hitreactmontage section depending when the hit is coming from
void ABaseCharacter::PlayHitReactMontage(FVector impact_point)
{
	double theta = GetDirectionalTheta(impact_point);

	FName section("FromBack");

	if (theta >= -45 && theta < 45)
	{
		section = FName("FromFront");
	}
	else if (theta >= -135 && theta < -45)
	{
		section = FName("FromLeft");
	}
	else if (theta >= 45 && theta < 135)
	{
		section = FName("FromRight");
	}

	PlayMontageSection(hit_react_montage, section);
}

// plays the appropriate deathmontage section depending on where the hit is coming from
void ABaseCharacter::PlayDyingMontage(FVector impact_point)
{
	if (!death_montage) return;
	FName section = GetDirectionMontageSection(impact_point);

	FString section_name = FString("Death") + section.ToString();

	TArray<FName> direction_montage_names;

	for (int i = 0; i < death_montage->CompositeSections.Num(); i++)
	{
		if (death_montage->GetSectionName(i).ToString().StartsWith(section_name))
		{
			direction_montage_names.Add(death_montage->GetSectionName(i));
		}
	}

	int32 random_selection = FMath::RandRange(0, direction_montage_names.Num() - 1);

	death_montage_section = direction_montage_names[random_selection];

	PlayMontageSection(death_montage, death_montage_section);
}

//get the appropriate section name to play when getting hit
FName ABaseCharacter::GetDirectionMontageSection(FVector impact_point)
{
	double theta = GetDirectionalTheta(impact_point);

	FName section("FromBack");

	if (theta >= -45 && theta < 45)
	{
		section = FName("FromFront");
	}
	else if (theta >= -135 && theta < -45)
	{
		section = FName("FromLeft");
	}
	else if (theta >= 45 && theta < 135)
	{
		section = FName("FromRight");
	}

	return section;
}

// stops the attack montage
void ABaseCharacter::StopAttackMontage()
{
	if (!equipped_weapon) return;
	UAnimInstance* anim_instance = GetMesh()->GetAnimInstance();
	if (anim_instance)
	{
		anim_instance->Montage_Stop(0.25f, attack_montages[static_cast<int>(equipped_weapon->GetWeaponType())]);
	}
}

//stops the dying montage
void ABaseCharacter::StopDyingMontage()
{
	UAnimInstance* anim_instance = GetMesh()->GetAnimInstance();
	if (anim_instance)
	{
		anim_instance->Montage_Stop(0.25f, death_montage);
	}
}

// warps to the target when attacking
void ABaseCharacter::WarpToTarget()
{
	if (combat_target && motion_warping && WithinDistanceFromTarget(combat_target, attack_radius))
	{
		motion_warping->AddOrUpdateWarpTargetFromLocation(FName("TranslationTarget"), GetTranslationWarpTarget());
		motion_warping->AddOrUpdateWarpTargetFromLocation(FName("RotationTarget"), GetRotationWarpTarget());
		//if (debug) UE_LOG(LogTemp, Warning, TEXT("Found combat target"));
	}
	else
	{
		motion_warping->RemoveWarpTarget(FName("TranslationTarget"));
		motion_warping->RemoveWarpTarget(FName("RotationTarget"));
		//if (debug) UE_LOG(LogTemp, Warning, TEXT("No combat target"));
	}
}

//get the angle between the attacker and the attackee
double ABaseCharacter::GetDirectionalTheta(FVector impact_point)
{
	FVector impact_point_lowered = FVector(impact_point.X, impact_point.Y, GetActorLocation().Z);
	FVector impact_vector = (impact_point_lowered - GetActorLocation()).GetSafeNormal();
	FVector actor_vector = GetActorForwardVector();

	double theta = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(actor_vector, impact_vector)));

	FVector cross_product = FVector::CrossProduct(actor_vector, impact_vector);
	if (cross_product.Z < 0)
	{
		theta *= -1;
	}

	return theta;
}

// function that handles death
void ABaseCharacter::Die_Implementation(FVector impact_point)
{
	PlayDyingMontage(impact_point);
	if (equipped_weapon && equipped_weapon->IsPickupable()) equipped_weapon->GetUnequipped();
	DisableCapsuleComponent();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponBoxCollision(ECollisionEnabled::NoCollision);
	StopAttackMontage();
	Tags.Add("Dead");
}

void ABaseCharacter::DisableCapsuleComponent()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::LifeSpanExpired()
{
	Destroy();
}

bool ABaseCharacter::IsAlive()
{
	return GetHealth() > 0;
}

bool ABaseCharacter::IsDead()
{
	return !IsAlive();
}

float ABaseCharacter::GetHealth()
{
	if (attributes) return attributes->GetHealth();

	return -1;
}

void ABaseCharacter::ReceiveDamage(float damage)
{
	if (attributes) attributes->ReceiveDamage(damage);
}

// does the attack action
void ABaseCharacter::Attack(const FInputActionValue& value)
{
	if (combat_target && combat_target->ActorHasTag(FName("Dead")))
	{
		combat_target = nullptr;
		if (debug) UE_LOG(LogTemp, Warning, TEXT("[Attack] combat target set to nullptr"));
	}
}

// get the target when the motion warping is in translation mode
FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (combat_target == nullptr) return FVector();

	const FVector combat_target_location = combat_target->GetActorLocation();
	const FVector location = GetActorLocation();

	FVector target_to_me = (location - combat_target_location).GetSafeNormal();
	target_to_me *= warp_target_distance;
	return combat_target_location + target_to_me;
}
 //gets the target when the motion warping is in rotation mode
FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (combat_target)
	{
		return combat_target->GetActorLocation();
	}
	return FVector();
}

// takes weapon from the ground
void ABaseCharacter::TakeWeapon(AWeapon* weapon)
{
	SetEquippedWeapon(weapon);
	AttachWeaponToSocket(socket_names[static_cast<int>(weapon->GetWeaponType())]);
}

//takes shield from the ground
void ABaseCharacter::TakeShield(AShield* shield)
{
	return;
}

// overriden function that handles receiving damage
float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ReceiveDamage(DamageAmount);
	UpdateHealthHUD();

	return DamageAmount;
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	WarpToTarget();
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// plays attack montage
void ABaseCharacter::PlayAttackMontage()
{
	PlayRandomMontageSection(attack_montages[static_cast<int>(equipped_weapon->GetWeaponType())]);
}

//attaches weapon to a socket
void ABaseCharacter::AttachWeaponToSocket(FName socket_name)
{
	AttachMeshToSocket(equipped_weapon, socket_name);
}

// sets the weapon box collision
void ABaseCharacter::SetWeaponBoxCollision(ECollisionEnabled::Type response) 
{
	if(equipped_weapon) equipped_weapon->GetWeaponBox()->SetCollisionEnabled(response); 
}

// enables the weapon box collision so that hits can be generated
void ABaseCharacter::EnableWeaponBoxCollision()
{
	if (equipped_weapon) equipped_weapon->EnableWeaponBoxCollision();
}

// disables weapon box collision so that hits are not generated when the character is not attacking
void ABaseCharacter::DisableWeaponBoxCollision()
{
	if (equipped_weapon) equipped_weapon->DisableWeaponBoxCollision();
}

// attaches a mesh to a socket
void ABaseCharacter::AttachMeshToSocket(AItem* item, FName socket_name)
{
	item->AttachMeshToSocket(GetMesh(), socket_name);
}

bool ABaseCharacter::WithinDistanceFromTarget(AActor* target, float radius)
{
	if (!target) return false;

	return UKismetMathLibrary::VSizeXY(GetActorLocation() - target->GetActorLocation()) <= radius;
}

// plays the appropriate hit sound according to weapon type
void ABaseCharacter::PlayHitSound(const FVector& impact_point, EWeaponDamageType type)
{
	if (hit_sounds[static_cast<int>(type)])
	{
		UGameplayStatics::PlaySoundAtLocation(this, hit_sounds[static_cast<int>(type)], impact_point);
	}
}

void ABaseCharacter::PlayPainSound()
{
	if (pain_sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, pain_sound, GetActorLocation());
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& impact_point)
{
	if (hit_particles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hit_particles, impact_point);
	}
}
