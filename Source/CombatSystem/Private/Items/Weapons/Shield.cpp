// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Shield.h"
#include "Characters/ControlledCharacter.h"
#include "Items/Weapons/WeaponEnums.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Items/Item.h"

AShield::AShield()
{
	shield_box = CreateDefaultSubobject<UBoxComponent>(TEXT("Shield Box"));
	shield_box->SetupAttachment(GetRootComponent());

	shield_box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	shield_box->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	shield_box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	shield_box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	shield_box->SetGenerateOverlapEvents(true);
}

void AShield::BeginPlay()
{
	Super::BeginPlay();

	shield_box->OnComponentBeginOverlap.AddDynamic(this, &AShield::OnShieldBoxOverlap);
}

void AShield::OnShieldBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWeapon* weapon = Cast<AWeapon>(OtherActor);
	if (weapon)
	{
		if (weapon->GetItemState() == EItemState::EIS_Hovering) return;
		weapon->AddToIgnored(GetOwner());
		if(AControlledCharacter* character = Cast<AControlledCharacter>(GetOwner())) character->BlockAttack();
	}
}

void AShield::GetEquipped(ABaseCharacter* actor)
{
	item_state = EItemState::EIS_Equipped;
	SetOwner(Cast<AActor>(actor));
	DisableSphereCollision();
	//shield_box->SetupAttachment(GetOwner()->GetRootComponent());
	actor->EquipShield(this);
	TurnOffUIDisplay();

	SetEquippedCollision();
}

void AShield::DisableShieldBoxCollision()
{
	shield_box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AShield::EnableShieldBoxCollision()
{
	shield_box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}