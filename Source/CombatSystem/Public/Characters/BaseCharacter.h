#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "InputActionValue.h"
#include "Items/Weapons/WeaponEnums.h"
#include "BaseCharacter.generated.h"

class UAnimMontage;
class UMotionWarpingComponent;
class UAttributes;
class USoundBase;
class UParticleSystem;
class AWeapon;

UCLASS(Abstract)
class COMBATSYSTEM_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	ABaseCharacter();
	virtual void GetHit(AActor* hitter, AWeapon* weapon, FVector impact_point) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void PlayAttackMontage();
	virtual void TakeWeapon(class AWeapon* weapon);
	virtual void TakeShield(class AShield* shield);
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToSocket(FName socket_name);

	UPROPERTY(EditDefaultsOnly)
	TArray<FName> socket_names;

	UFUNCTION(BlueprintCallable)
	void AttachMeshToSocket(AItem* item, FName socket_name);

protected:
	virtual void BeginPlay() override;
	void PlayMontageSection(UAnimMontage* montage, FName section);
	void PlayRandomMontageSection(UAnimMontage* montage);
	void PlayHitReactMontage(FVector impact_point);
	void PlayDyingMontage(FVector impact_point);
	FName GetDirectionMontageSection(FVector impact_point);
	void StopAttackMontage();
	void StopDyingMontage();
	void WarpToTarget();
	double GetDirectionalTheta(FVector impact_point);
	UFUNCTION(BlueprintNativeEvent)
	void Die(FVector impact_point);
	void DisableCapsuleComponent();
	void LifeSpanExpired();
	bool IsAlive();
	bool IsDead();
	float GetHealth();
	void ReceiveDamage(float damage);
	virtual void UpdateHealthHUD() PURE_VIRTUAL(ABaseCharacter::UpdateHealthHUD );
	virtual void Attack(const FInputActionValue&);
	bool WithinDistanceFromTarget(AActor* target, float radius);
	void PlayHitSound(const FVector& impact_point, EWeaponDamageType damage_type);
	void PlayPainSound();
	void SpawnHitParticles(const FVector&);

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	AWeapon* equipped_weapon;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* hit_react_montage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	TArray<UAnimMontage*> attack_montages;

	UPROPERTY(EditDefaultsOnly, Category = Death)
	UAnimMontage* death_montage;

	UPROPERTY(BlueprintReadOnly, Category = Death)
	FName death_montage_section;

	UPROPERTY(EditAnywhere, Category = Combat)
	double warp_target_distance = 75.f;

	UPROPERTY(BlueprintReadOnly)
	AActor* combat_target;

	UPROPERTY(BlueprintReadOnly)
	UMotionWarpingComponent* motion_warping;

	UPROPERTY(VisibleAnywhere)
	UAttributes* attributes;

	UPROPERTY(EditAnywhere)
	double attack_radius = 150.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<USoundBase*> hit_sounds;

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* pain_sound;

	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* hit_particles;

	UPROPERTY(EditAnywhere)
	float damage_multiplier = 1;

public:
	UFUNCTION(BlueprintCallable)
	void SetWeaponBoxCollision(ECollisionEnabled::Type response);

	UFUNCTION(BlueprintCallable)
	void EnableWeaponBoxCollision();

	UFUNCTION(BlueprintCallable)
	void DisableWeaponBoxCollision();

	FORCEINLINE AWeapon* GetEquippedWeapon() const { return equipped_weapon; }
	FORCEINLINE void SetEquippedWeapon(AWeapon* weapon) { equipped_weapon = weapon; }

	FORCEINLINE float GetDamageMultiplier() { return damage_multiplier; }
};