// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "WeaponEnums.h"
#include "Weapon.generated.h"

class UBoxComponent;
class ABaseCharacter;
/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	void GetEquipped(ABaseCharacter* actor) override;
	virtual void GetUnequipped();
	AWeapon();

	void DisableWeaponBoxCollision();
	void EnableWeaponBoxCollision();
	void EnableGhostWeaponBoxCollision();
	bool IsPickupable() { return is_pickupable; };
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly)
	EWeaponType weapon_type;

	UPROPERTY(EditDefaultsOnly)
	EWeaponDamageType weapon_damage_type;

	UPROPERTY(EditDefaultsOnly)
	bool is_pickupable = true;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGhostBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* weapon_box;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* ghost_weapon_box;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* box_trace_start;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* box_trace_end;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector box_trace_extent = FVector(5.f);

	void BoxTrace(FHitResult& box_hit);

	TArray<AActor*> ignored_actors;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float damage = 20;

public:
	FORCEINLINE EWeaponType GetWeaponType() { return weapon_type; }
	FORCEINLINE EWeaponDamageType GetWeaponDamageType() { return weapon_damage_type; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UBoxComponent* GetWeaponBox() { return weapon_box; }
	FORCEINLINE void AddToIgnored(AActor* actor) { ignored_actors.AddUnique(actor); }
};
