// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"


class USphereComponent;

enum class EItemState
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class COMBATSYSTEM_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

	virtual void GetEquipped(class ABaseCharacter* actor);
	void DisableSphereCollision();
	void EnableSphereCollision();
	void AttachMeshToSocket(USceneComponent* parent_mesh, FName socket_name);
	void TurnOnUIDisplay();
	void TurnOffUIDisplay();
	void SetUnequippedCollision();
	void SetEquippedCollision();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float amplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float time_constant;

	EItemState item_state = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* item_mesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* sphere;

	UPROPERTY(VisibleAnywhere)
	class UItemUIComponent* item_ui_widget;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	float running_time = 0;

public:
	FORCEINLINE void SetItemState(EItemState state) { item_state = state; }
	FORCEINLINE EItemState GetItemState() { return item_state; }
};
