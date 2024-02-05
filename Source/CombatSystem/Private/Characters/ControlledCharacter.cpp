// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ControlledCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Animation/AnimMontage.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Weapons/Shield.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/BoxComponent.h"
#include "HUD/CombatSystemHUD.h"
#include "HUD/ControlledCharacterOverlay.h"
#include "Components/Attributes.h"
#include "GroomComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AControlledCharacter::AControlledCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	camera_boom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	camera_boom->SetupAttachment(GetRootComponent());
	camera_boom->TargetArmLength = 300.f;
	camera_boom->bUsePawnControlRotation = true;

	view_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	view_camera->SetupAttachment(camera_boom);
	view_camera->bUsePawnControlRotation = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 400, 0);

	attack_sphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	attack_sphere->SetupAttachment(GetRootComponent());

	focused_camera_point = CreateDefaultSubobject<USceneComponent>(TEXT("FocusCameraPoint"));
	focused_camera_point->SetupAttachment(RootComponent);

	hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	hair->SetupAttachment(GetMesh());
	hair->AttachmentName = FString("head");

	eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	eyebrows->SetupAttachment(GetMesh());
	eyebrows->AttachmentName = FString("head");
}

// Called when the game starts or when spawned
void AControlledCharacter::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(FName("EngageableTarget"));

	if (APlayerController* player_controller = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(player_controller->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(standard_mapping_context, 0);
		}
	}
	
	attack_sphere->OnComponentBeginOverlap.AddDynamic(this, &AControlledCharacter::OnAttackSphereBeginOverlap);
	attack_sphere->OnComponentEndOverlap.AddDynamic(this, &AControlledCharacter::OnAttackSphereEndOverlap);

	GetCharacterMovement()->MaxWalkSpeed = running_speed;

	APlayerController* player_controller = Cast<APlayerController>(GetController());

	if (player_controller)
	{
		ACombatSystemHUD* hud = Cast<ACombatSystemHUD>(player_controller->GetHUD());
		if (hud)
		{
			hud_overlay = hud->GetCombatSystemOverlay();
			if (hud_overlay && attributes)
			{
				hud_overlay->SetHealthPercent(attributes->GetHealthPercent());
				hud_overlay->SetStaminaPercent(attributes->GetStaminaPercent());
			}
		}
	}
	respawn_point = GetActorLocation();
}

// Called every frame
void AControlledCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));

	if (action_mode == EMode::EM_Sprinting && FVector2D(GetCharacterMovement()->Velocity).Size() > 0 && (action_state == EActionState::EAS_Unoccupied || action_state == EActionState::EAS_Dodging))
	{
		attributes->UseStaminaThroughSprint();
		UpdateStaminaHUD();
		if (GetStamina() == 0)
		{
			SetModeToRecoveringStamina();
		}
	}
	else
	{
		attributes->RegenerateStamina();
		UpdateStaminaHUD();

		if (IsStaminaFull())
		{
			mode_conditions.recovering_stamina = false;
			if (action_mode == EMode::EM_RecoveringStamina)
			{
				SetModeToStandard();
			}
		}
	}

	if (action_mode == EMode::EM_Engaged)
	{
		SetEngagedCamera();
	}

	if (mode_conditions.recovering_stamina)
	{
		FlashStaminaBar();
	}

	if (action_state == EActionState::EAS_Rolling && !is_rolling)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *target_roll_rotation.ToString());
		if (GetActorRotation().Equals(target_roll_rotation, 1.f))
		{
			PlayMontageSection(focus_dodge_montage, FName("Roll"));
			is_rolling = true;
		}
		else
		{
			FRotator delta_rotation = FMath::RInterpTo(GetActorRotation(), target_roll_rotation, DeltaTime, roll_rotation_interp_rate);

			SetActorRotation(delta_rotation);
		}

	}

	SetOverlappingItemUI();
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::SanitizeFloat(overlapping_items.Num()));
}

// Called to bind functionality to input
void AControlledCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhanced_input_component = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhanced_input_component->BindAction(move_forward_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::MoveForward);
		enhanced_input_component->BindAction(move_right_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::MoveRight);
		enhanced_input_component->BindAction(turn_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::Turn);
		enhanced_input_component->BindAction(look_up_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::LookUp);
		enhanced_input_component->BindAction(jump_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::CharacterJump);
		enhanced_input_component->BindAction(attack_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::Attack);

		enhanced_input_component->BindAction(take_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::EKeyPressed);
		enhanced_input_component->BindAction(equip_weapon_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::RKeyPressed);
		enhanced_input_component->BindAction(equip_shield_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::TKeyPressed);
		enhanced_input_component->BindAction(focus_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::FocusOnEnemy);
		enhanced_input_component->BindAction(unfocus_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::UnfocusOnEnemy);
		enhanced_input_component->BindAction(dodge_right_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::DodgeRight);
		enhanced_input_component->BindAction(dodge_forward_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::DodgeForward);
		enhanced_input_component->BindAction(sprint_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::Sprint);
		enhanced_input_component->BindAction(sprint_end_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::SprintEnd);
		enhanced_input_component->BindAction(duration_of_sprint_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::DurationOfSprint);
		enhanced_input_component->BindAction(roll_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::Roll);
		enhanced_input_component->BindAction(toggle_controls_display_input_action, ETriggerEvent::Triggered, this, &AControlledCharacter::ToggleControlsDisplay);
	}
}

void AControlledCharacter::MoveForward(const FInputActionValue& input_value)
{
	if (action_state != EActionState::EAS_Unoccupied && action_state != EActionState::EAS_Equipping) return;

	const float value = input_value.Get<float>();
	forward_input = value;
	if (Controller && value != 0.f)
	{
		const FRotator control_rotation = GetControlRotation();
		const FRotator yaw_rotation(0.f, control_rotation.Yaw, 0.f);
		const FVector direction = FRotationMatrix(yaw_rotation).GetUnitAxis(EAxis::X);
		AddMovementInput(direction, value);
	}
}

void AControlledCharacter::MoveRight(const FInputActionValue& input_value)
{
	if (action_state != EActionState::EAS_Unoccupied && action_state != EActionState::EAS_Equipping) return;
	const float value = input_value.Get<float>();
	right_input = value;
	if (Controller && value != 0.f)
	{
		const FRotator control_rotation = GetControlRotation();
		const FRotator yaw_rotation(0.f, control_rotation.Yaw, 0.f);

		const FVector direction = FRotationMatrix(yaw_rotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(direction, value);
	}
}

void AControlledCharacter::Turn(const FInputActionValue& input_value)
{
	if (IsDead()) return;
	if (action_mode != EMode::EM_Engaged)
	{
		const float value = input_value.Get<float>();
		AddControllerYawInput(value);
	}
}

void AControlledCharacter::LookUp(const FInputActionValue& input_value)
{
	if (IsDead()) return;
	if (action_mode != EMode::EM_Engaged) {
		const float value = input_value.Get<float>();
		AddControllerPitchInput(value);
	}
}

void AControlledCharacter::CharacterJump(const FInputActionValue& input_value)
{
	if (action_state == EActionState::EAS_Unoccupied && !GetCharacterMovement()->IsFalling())
	{
		Super::Jump();
		action_state = EActionState::EAS_Dodging;
		if (character_shield_state == ECharacterShieldState::ECSS_EquippedShield) DisableShieldCollision();
	}
}

void AControlledCharacter::EKeyPressed(const FInputActionValue& input_value)
{
	if (overlapping_item)
	{
		if (equipped_weapon && Cast<AWeapon>(overlapping_item))
		{
			equipped_weapon->Destroy();
		}
		else if (equipped_shield && Cast<AShield>(overlapping_item))
		{
			equipped_shield->Destroy();
		}
		overlapping_item->GetEquipped(this);
	}
}

void AControlledCharacter::Attack(const FInputActionValue& input_value)
{
	Super::Attack(input_value);
	if (action_state != EActionState::EAS_Unoccupied) return;
	if (character_weapon_state != ECharacterWeaponState::ESC_Unequipped)
	{
		PlayAttackMontage();
		action_state = EActionState::EAS_Attacking;
		if(equipped_shield) DisableShieldCollision();
	}
}

void AControlledCharacter::GetHit(AActor* hitter, AWeapon* weapon, FVector impact_point)
{
	Super::GetHit(hitter, weapon, impact_point);
	if (IsAlive())
	{
		action_state = EActionState::EAS_HitReaction;
		if (character_shield_state == ECharacterShieldState::ECSS_EquippedShield) DisableShieldCollision();
		if (!combat_target) combat_target = hitter;
	}
}

void AControlledCharacter::AttachShieldToSocket(FName socket_name)
{
	if(equipped_shield) AttachMeshToSocket(equipped_shield, socket_name);
}

void AControlledCharacter::EquipWeapon()
{
	AttachWeaponToSocket(socket_names[static_cast<int>(equipped_weapon->GetWeaponType())]);
}

void AControlledCharacter::GetHealthBoost(float health_boost)
{
	if (attributes) {
		attributes->GetHealthBoost(health_boost);
		ShowHealthRise(health_boost);
	}
}

void AControlledCharacter::GetStaminaBoost(float stamina_boost)
{
	if (attributes)
	{
		attributes->GetStaminaBoost(stamina_boost);
		AnimateStaminaHUD(stamina_boost);
	}
}

void AControlledCharacter::RKeyPressed(const FInputActionValue& input_value)
{
	if (!equipped_weapon) return;
	if (action_state != EActionState::EAS_Unoccupied) return;
	if (character_weapon_state == ECharacterWeaponState::ESC_Unequipped)
	{
		if (equipped_weapon->GetWeaponType() > EWeaponType::EWT_OneHandedWeapon && equipped_shield && character_shield_state == ECharacterShieldState::ECSS_EquippedShield)
		{
			PlayMontageSection(arm_disarm_montages[static_cast<int>(equipped_weapon->GetWeaponType())], FName("UnequipShieldThenEquip"));
			character_shield_state = ECharacterShieldState::ECSS_Unequipped;
		}
		else
		{
			PlayMontageSection(arm_disarm_montages[static_cast<int>(equipped_weapon->GetWeaponType())], FName("Arm"));
		}
		character_weapon_state = static_cast<ECharacterWeaponState>(static_cast<int>(equipped_weapon->GetWeaponType()) + 1);
		action_state = EActionState::EAS_Equipping;
	}
	else
	{
		PlayMontageSection(arm_disarm_montages[static_cast<int>(equipped_weapon->GetWeaponType())], FName("Disarm"));
		character_weapon_state = ECharacterWeaponState::ESC_Unequipped;
		action_state = EActionState::EAS_Equipping;
	}
}

void AControlledCharacter::TKeyPressed(const FInputActionValue& input_value)
{
	if (!equipped_shield) return;
	if (action_state != EActionState::EAS_Unoccupied) return;

	if (character_shield_state == ECharacterShieldState::ECSS_Unequipped)
	{
		if (equipped_weapon && character_weapon_state > ECharacterWeaponState::ESC_EquippedOneHandedWeapon)
		{
			PlayMontageSection(shield_arm_disarm_montage, FName("UnequipWeaponThenEquip"));
			character_weapon_state = ECharacterWeaponState::ESC_Unequipped;
			DisableShieldCollision();
		}
		else
		{
			PlayMontageSection(shield_arm_disarm_montage, FName("Arm"));
			if (action_mode == EMode::EM_Engaged) EnableShieldCollision();
		}
		action_state = EActionState::EAS_Equipping;
		character_shield_state = ECharacterShieldState::ECSS_EquippedShield;
	}
	else
	{
		PlayMontageSection(shield_arm_disarm_montage, FName("Disarm"));
		character_shield_state = ECharacterShieldState::ECSS_Unequipped;
		action_state = EActionState::EAS_Equipping;
	}
}

void AControlledCharacter::FocusOnEnemy(const FInputActionValue& input_value)
{
	if (IsDead()) return;
	SetModeToEngaged();
	if (character_shield_state == ECharacterShieldState::ECSS_EquippedShield) EnableShieldCollision();

	combat_target = ReturnClosestEnemy();
}

void AControlledCharacter::UnfocusOnEnemy(const FInputActionValue& input_value)
{
	combat_target = nullptr;
	mode_conditions.engaged = false;
	if (mode_conditions.recovering_stamina)
	{
		SetModeToRecoveringStamina();
	}
	else if (mode_conditions.sprinting)
	{
		SetModeToSprinting();
	}
	else
	{
		SetModeToStandard();
	}
	
	if (character_shield_state == ECharacterShieldState::ECSS_EquippedShield) DisableShieldCollision();
}

void AControlledCharacter::DodgeRight(const FInputActionValue& value)
{
	if (action_mode != EMode::EM_Engaged)
	{
		CharacterJump(value);
		return;
	}

	if (action_state != EActionState::EAS_Unoccupied || GetCharacterMovement()->IsFalling()) return;

	if (character_shield_state == ECharacterShieldState::ECSS_EquippedShield) DisableShieldCollision();
	action_state = EActionState::EAS_Dodging;
	FVector2D ground_velocity = FVector2D(GetActorRotation().UnrotateVector(GetVelocity()));

	if (right_input > 0)
	{
		PlayMontageSection(focus_dodge_montage, FName("DodgeRight"));
	}
	else if(right_input < 0)
	{
		PlayMontageSection(focus_dodge_montage, FName("DodgeLeft"));
	}
}

void AControlledCharacter::DodgeForward(const FInputActionValue& value)
{
	if (action_mode != EMode::EM_Engaged)
	{
		CharacterJump(value);
		return;
	}

	if (action_state != EActionState::EAS_Unoccupied || GetCharacterMovement()->IsFalling()) return;

	if (character_shield_state == ECharacterShieldState::ECSS_EquippedShield) DisableShieldCollision();
	action_state = EActionState::EAS_Dodging;
	FVector2D ground_velocity = FVector2D(GetActorRotation().UnrotateVector(GetVelocity()));

	if (forward_input > 0)
	{
		PlayMontageSection(focus_dodge_montage, FName("DodgeForward"));
	}
	else if (forward_input < 0)
	{
		PlayMontageSection(focus_dodge_montage, FName("DodgeBackward"));
	}
}

void AControlledCharacter::Sprint(const FInputActionValue& value)
{
	if (mode_conditions.recovering_stamina) return;
	SetModeToSprinting();
}

void AControlledCharacter::SprintEnd(const FInputActionValue& value)
{
	if (mode_conditions.recovering_stamina) return;
	mode_conditions.sprinting = false;
	if (mode_conditions.engaged)
	{
		SetModeToEngaged();
	}
	else
	{
		SetModeToStandard();
	}
}

void AControlledCharacter::DurationOfSprint(const FInputActionValue& value)
{
	if (UKismetMathLibrary::VSizeXY(GetCharacterMovement()->Velocity) > 0)
	{
		UnequipWeapon();
		UnequipShield();
	}
}

void AControlledCharacter::Roll(const FInputActionValue& value)
{
	if (!CanRoll()) return;
	if (action_state == EActionState::EAS_Unoccupied && !GetCharacterMovement()->IsFalling())
	{
		target_roll_rotation = FRotator(GetActorRotation().Roll, GetController()->GetControlRotation().Yaw, GetActorRotation().Pitch);
		action_state = EActionState::EAS_Rolling;
		attributes->UseStaminaThroughRoll();
		UpdateStaminaHUD();
		if (GetStamina() == 0)
		{
			SetModeToRecoveringStamina();
		}
	}
}

void AControlledCharacter::ToggleControlsDisplay(const FInputActionValue& value)
{
	if (hud_overlay) hud_overlay->ToggleControlsDisplay();
}

void AControlledCharacter::BlockAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Block reaction started"));
	if (hit_react_montage && action_state != EActionState::EAS_HitReaction)
	{
		PlayMontageSection(hit_react_montage, FName("BlockedReaction"));
		action_state = EActionState::EAS_HitReaction;
	}
}

float AControlledCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ShowHealthFall(-DamageAmount);

	return DamageAmount;
}

void AControlledCharacter::EquipWeapon(AWeapon* weapon)
{
	Super::EquipWeapon(weapon);

	SetCharacterWeaponState(static_cast<ECharacterWeaponState>(static_cast<int>(weapon->GetWeaponType()) + 1));

	if (GetEquippedShield() && equipped_weapon->GetWeaponType() == EWeaponType::EWT_TwoHandedWeapon)
	{
		AttachShieldToSocket("ShieldSocket");
		SetCharacterShieldState(ECharacterShieldState::ECSS_Unequipped);
	}
}

void AControlledCharacter::EquipShield(AShield* shield)
{
	Super::EquipShield(shield);

	AttachMeshToSocket(shield, FName("LeftHandSocket"));
	SetCharacterShieldState(ECharacterShieldState::ECSS_EquippedShield);
	SetEquippedShield(shield);
	if (equipped_weapon && equipped_weapon->GetWeaponType() > EWeaponType::EWT_OneHandedWeapon)
	{
		AttachWeaponToSocket(FName("NeckSocket"));
		SetCharacterWeaponState(ECharacterWeaponState::ESC_Unequipped);
	}
}

void AControlledCharacter::UnequipWeapon()
{
	if (equipped_weapon && character_weapon_state != ECharacterWeaponState::ESC_Unequipped)
	{
		AttachWeaponToSocket(FName("NeckSocket"));
		character_weapon_state = ECharacterWeaponState::ESC_Unequipped;
	}
}

void AControlledCharacter::UnequipShield()
{
	if (equipped_shield && character_shield_state != ECharacterShieldState::ECSS_Unequipped)
	{
		AttachShieldToSocket(FName("ShieldSocket"));
		character_shield_state = ECharacterShieldState::ECSS_Unequipped;
	}
}

void AControlledCharacter::UpdateHealthHUD()
{
	if (hud_overlay && attributes) hud_overlay->SetHealthPercent(attributes->GetHealthPercent());
}

void AControlledCharacter::UpdateShadowHealthHUD()
{
	if (hud_overlay && attributes) hud_overlay->SetShadowHealthPercent(attributes->GetHealthPercent());
}

void AControlledCharacter::ShowHealthRise(float health_increment)
{
	UpdateShadowHealthHUD();
	ShowHealthIncrement(health_increment);
	FollowShadowHealthBar();
}

void AControlledCharacter::ShowHealthFall(float health_decrement)
{
	UpdateHealthHUD();
	ShowHealthIncrement(health_decrement);
	FollowHealthBar();
}

void AControlledCharacter::AnimateStaminaHUD(float stamina_change)
{
	UpdateStaminaHUD();
	ShowStaminaIncrement(stamina_change);
}

void AControlledCharacter::Die_Implementation(FVector impact_point)
{
	Super::Die_Implementation(impact_point);
	action_state = EActionState::EAS_Dead;

	GetWorldTimerManager().SetTimer(death_screen_timer, this, &AControlledCharacter::SetDeathScreen, death_screen_time);
	GetWorldTimerManager().SetTimer(respawn_timer, this, &AControlledCharacter::ReloadLevel, respawn_time);
}

void AControlledCharacter::DisableShieldCollision()
{
	if (equipped_shield) equipped_shield->DisableShieldBoxCollision();
}

void AControlledCharacter::EnableShieldCollision()
{
	if (equipped_shield) equipped_shield->EnableShieldBoxCollision();
}

void AControlledCharacter::UpdateStaminaHUD()
{
	if (hud_overlay && attributes) hud_overlay->SetStaminaPercent(attributes->GetStaminaPercent());
}

void AControlledCharacter::SetEngagedCamera()
{
	if (GetController()->GetControlRotation() != focused_camera_point->GetComponentRotation())
	{
		if (UWorld* world = GetWorld())
		{
			if (combat_target)
			{
				FRotator to_combat_target_location = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), combat_target->GetActorLocation());
				FRotator delta_combat_target_rotation = FMath::RInterpTo(GetActorRotation(), to_combat_target_location, world->GetDeltaSeconds(), camera_interp_rate);
				SetActorRotation(delta_combat_target_rotation);
			}
			FRotator delta_rotation = FMath::RInterpTo(GetController()->GetControlRotation(), focused_camera_point->GetComponentRotation(), world->GetDeltaSeconds(), camera_interp_rate);
			GetController()->SetControlRotation(delta_rotation);
		}
	}
}

float AControlledCharacter::GetStamina()
{
	if (attributes) return attributes->GetStamina();
	return -1;
}

bool AControlledCharacter::IsStaminaFull()
{
	if (attributes) return attributes->GetStaminaPercent() == 1.f;

	return false;
}

void AControlledCharacter::SetModeToStandard()
{
	action_mode = EMode::EM_Standard;
	mode_conditions.recovering_stamina = false;
	mode_conditions.sprinting = false;
	mode_conditions.engaged = false;
	SetSpeed(running_speed);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	SetStaminaOpacityToFull();
}

void AControlledCharacter::SetModeToEngaged()
{
	action_mode = EMode::EM_Engaged;
	mode_conditions.engaged = true;
	SetSpeed(engaged_speed);
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AControlledCharacter::SetModeToSprinting()
{
	action_mode = EMode::EM_Sprinting;
	mode_conditions.sprinting = true;
	mode_conditions.recovering_stamina = false;
	SetSpeed(sprinting_speed);
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AControlledCharacter::SetModeToRecoveringStamina()
{
	action_mode = EMode::EM_RecoveringStamina;
	mode_conditions.recovering_stamina = true;
	mode_conditions.sprinting = false;
	SetSpeed(recovering_stamina_speed);
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AControlledCharacter::SetSpeed(float speed)
{
	GetCharacterMovement()->MaxWalkSpeed = speed;
}

void AControlledCharacter::FlashStaminaBar()
{
	if (hud_overlay) hud_overlay->FlashStaminaBar();
}

void AControlledCharacter::SetStaminaOpacityToFull()
{
	if (hud_overlay) hud_overlay->SetStaminaOpacityToFull();
}

bool AControlledCharacter::CanRoll()
{
	if (action_mode == EMode::EM_RecoveringStamina) return false;
	if (attributes) return attributes->CanUseStamina(attributes->GetRollCost());

	return false;
}

void AControlledCharacter::ReSpawn()
{
	StopDyingMontage();
	SetActorLocation(respawn_point);
	if (attributes) attributes->Respawn();
	UpdateHealthHUD();
	UpdateStaminaHUD();
	action_state = EActionState::EAS_Unoccupied;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Tags.Remove("Dead");
}

void AControlledCharacter::ReloadLevel()
{
	UGameplayStatics::OpenLevel(this, FName(GetWorld()->GetName()), false);
}

void AControlledCharacter::SetDeathScreen()
{
	if (hud_overlay) hud_overlay->TurnOnDeathImage();
}

void AControlledCharacter::SetOverlappingItemUI()
{
	if (overlapping_item) overlapping_item->TurnOffUIDisplay();
	float distance = INFINITY;
	AItem* closest_item = nullptr;
	for (AItem* item : overlapping_items)
	{
		if ((item->GetActorLocation() - GetActorLocation()).Size() < distance)
		{
			closest_item = item;
			distance = (item->GetActorLocation() - GetActorLocation()).Size();
		}
	}

	overlapping_item = closest_item;
	if (overlapping_item) overlapping_item->TurnOnUIDisplay();
}

void AControlledCharacter::ShowHealthIncrement(float health_change)
{
	if (hud_overlay) hud_overlay->ShowHealthIncrement(health_change);
}

void AControlledCharacter::ShowStaminaIncrement(float stamina_change)
{
	if (hud_overlay) hud_overlay->ShowStaminaIncrement(stamina_change);
}

void AControlledCharacter::FollowHealthBar()
{
	if (hud_overlay) hud_overlay->FollowHealthBar();
}

void AControlledCharacter::FollowShadowHealthBar()
{
	if (hud_overlay) hud_overlay->FollowShadowHealthBar();
}

void AControlledCharacter::OnAttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Dead")) return;
	if (OtherActor->ActorHasTag(FName("Enemy")))
	{
		enemies_in_range.AddUnique(OtherActor);
	}
}

void AControlledCharacter::OnAttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("Enemy")))
	{
		enemies_in_range.Remove(OtherActor);
		if (combat_target == OtherActor) combat_target = ReturnClosestEnemy();
	}
}

void AControlledCharacter::HitReactEnd()
{
	action_state = EActionState::EAS_Unoccupied;
	if (action_mode == EMode::EM_Engaged && character_shield_state == ECharacterShieldState::ECSS_EquippedShield) EnableShieldCollision();
	is_rolling = false;
	UE_LOG(LogTemp, Warning, TEXT("Hit react ended"));
}

void AControlledCharacter::DodgeEnd()
{
	action_state = EActionState::EAS_Unoccupied;
	if (action_mode == EMode::EM_Engaged && character_shield_state == ECharacterShieldState::ECSS_EquippedShield) EnableShieldCollision();
	is_rolling = false;
}

void AControlledCharacter::AttackEnd()
{
	action_state = EActionState::EAS_Unoccupied;
	if (action_mode == EMode::EM_Engaged && character_shield_state == ECharacterShieldState::ECSS_EquippedShield) EnableShieldCollision();
}

AActor* AControlledCharacter::ReturnClosestEnemy()
{
	for (AActor* enemy : enemies_in_range)
	{
		if (enemy->ActorHasTag("Dead")) enemies_in_range.Remove(enemy);
	}

	if (enemies_in_range.Num() == 0) return nullptr;

	AActor* closest_enemy = enemies_in_range[0];

	FVector my_location = GetActorLocation();

	for (AActor* enemy : enemies_in_range)
	{

		if ((closest_enemy->GetActorLocation() - my_location).Size() > (enemy->GetActorLocation() - my_location).Size())
		{
			closest_enemy = enemy;
		}
	}

	return closest_enemy;
}

void AControlledCharacter::SetOverlappingItem(AItem* item)
{
	overlapping_items.AddUnique(item);
}

void AControlledCharacter::RemoveOverlappingItem(AItem* item)
{
	overlapping_items.Remove(item);
}

AShield* AControlledCharacter::GetEquippedShield() const
{
	return equipped_shield;;
}

void AControlledCharacter::SetEquippedShield(AShield* shield)
{
	equipped_shield = shield;
}