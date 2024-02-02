// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/PickupInterface.h"
#include "Characters/CharacterEnums.h"
#include "InputActionValue.h"
#include "ControlledCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class AWeapon;
class AShield;
class USphereComponent;
class UInputMappingContext;
class UInputAction;
class UGroomComponent;

USTRUCT(BlueprintType)
struct FModeConditions
{
	GENERATED_BODY()
	bool sprinting = false;
	bool engaged = false;
	bool recovering_stamina = false;
};

UCLASS()
class COMBATSYSTEM_API AControlledCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	AControlledCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Attack(const FInputActionValue& input_value) override;
	virtual void GetHit(AActor* hitter, AWeapon* weapon, FVector impact_point) override;
	void BlockAttack();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void AttachShieldToSocket(FName socket_name);

	UFUNCTION(BlueprintCallable)
	void EquipWeapon();

protected:
	virtual void BeginPlay() override;
	void MoveForward(const FInputActionValue& value);
	void MoveRight(const FInputActionValue& value);
	void Turn(const FInputActionValue& value);
	void LookUp(const FInputActionValue& value);
	void CharacterJump(const FInputActionValue& value);
	void EKeyPressed(const FInputActionValue& value);
	void RKeyPressed(const FInputActionValue& value);
	void TKeyPressed(const FInputActionValue& value);
	void FocusOnEnemy(const FInputActionValue& value);
	void UnfocusOnEnemy(const FInputActionValue& value);
	void DodgeRight(const FInputActionValue& value);
	void DodgeForward(const FInputActionValue& value);
	void Sprint(const FInputActionValue& value);
	void SprintEnd(const FInputActionValue& value);
	void DurationOfSprint(const FInputActionValue& value);
	void Roll(const FInputActionValue& value);
	void ToggleControlsDisplay(const FInputActionValue& value);
	virtual void EquipWeapon(AWeapon* weapon) override;
	virtual void EquipShield(AShield* shield) override;
	void UnequipWeapon();
	void UnequipShield();
	virtual void UpdateHealthHUD() override;
	virtual void Die_Implementation(FVector) override;
	void DisableShieldCollision();
	void EnableShieldCollision();
	void UpdateStaminaHUD();
	void SetEngagedCamera();
	float GetStamina();
	bool IsStaminaFull();
	void SetModeToStandard();
	void SetModeToEngaged();
	void SetModeToSprinting();
	void SetModeToRecoveringStamina();
	void SetSpeed(float);
	void FlashStaminaBar();
	void SetStaminaOpacityToFull();
	bool CanRoll();
	void ReSpawn();
	void ReloadLevel();
	void SetDeathScreen();

	UFUNCTION()
	virtual void OnAttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnAttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UFUNCTION(BlueprintCallable)
	void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	
	EActionState action_state = EActionState::EAS_Unoccupied;

	ECharacterWeaponState character_weapon_state = ECharacterWeaponState::ESC_Unequipped;

	ECharacterShieldState character_shield_state = ECharacterShieldState::ECSS_Unequipped;

	EMode action_mode = EMode::EM_Standard;

	AShield* equipped_shield;

	TArray<AActor*> enemies_in_range;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* attack_sphere;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USceneComponent* focused_camera_point;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USpringArmComponent* camera_boom;

	UPROPERTY(EditAnywhere)
	float camera_interp_rate = 1;

	UPROPERTY(EditAnywhere)
	float roll_rotation_interp_rate = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* standard_mapping_context;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* move_forward_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* move_right_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* turn_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* look_up_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* jump_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* attack_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* take_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* equip_weapon_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* equip_shield_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* focus_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* unfocus_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* dodge_right_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* dodge_forward_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* sprint_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* sprint_end_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* duration_of_sprint_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* roll_input_action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* toggle_controls_display_input_action;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool is_sprinting;

	UPROPERTY(BlueprintReadOnly)
	struct FModeConditions mode_conditions;

	UPROPERTY(BlueprintReadOnly)
	bool is_shift_down = false;

	UPROPERTY(BlueprintReadOnly)
	bool is_right_mouse_down = false;

	UPROPERTY(BlueprintReadOnly)
	bool recovering_stamina = false;

	UPROPERTY(BlueprintReadOnly)
	FRotator target_roll_rotation;

	UPROPERTY(EditAnywhere)
	FVector respawn_point;

private:

	FTimerHandle respawn_timer;
	FTimerHandle death_screen_timer;

	UPROPERTY(EditAnywhere)
	float respawn_time;

	UPROPERTY(EditAnywhere)
	float death_screen_time;

	float forward_input = 0;
	float right_input = 0;

	bool is_rolling = false;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* view_camera;

	class AItem* overlapping_item;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	TArray<UAnimMontage*> arm_disarm_montages;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* shield_arm_disarm_montage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* focus_dodge_montage;

	UPROPERTY(EditAnywhere, Category = "Patrolling")
	float engaged_speed = 125.f;

	UPROPERTY(EditAnywhere, Category = "Chasing")
	float running_speed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Sprinting")
	float sprinting_speed = 450.f;

	UPROPERTY(EditAnywhere)
	float recovering_stamina_speed = 200;

	UPROPERTY(EditAnywhere, Category = "HUD")
	class UControlledCharacterOverlay* hud_overlay;

	AActor* ReturnClosestEnemy();

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* eyebrows;
public:

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EActionState GetActionState() const { return action_state; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetActionState(EActionState state) { action_state = state; }
	
	FORCEINLINE void SetOverlappingItem(AItem* item) { overlapping_item = item; }	

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ECharacterWeaponState GetCharacterWeaponState() const { return character_weapon_state; }
	FORCEINLINE void SetCharacterWeaponState(ECharacterWeaponState state) { character_weapon_state = state; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ECharacterShieldState GetCharacterShieldState() const { return character_shield_state; }
	FORCEINLINE void SetCharacterShieldState(ECharacterShieldState state) { character_shield_state = state; }

	FORCEINLINE EMode GetActionMode() { return action_mode; }
	FORCEINLINE void SetActionMode(EMode mode) { action_mode = mode; }

	AShield* GetEquippedShield() const;
	void SetEquippedShield(AShield* shield);
};
