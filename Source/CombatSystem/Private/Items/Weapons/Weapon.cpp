// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "Characters/BaseCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Item.h"
#include "HUD/ItemUIComponent.h"


AWeapon::AWeapon()
{
	weapon_box = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	weapon_box->SetupAttachment(GetRootComponent());
	weapon_box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	weapon_box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	weapon_box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	/*ghost_weapon_box = CreateDefaultSubobject<UBoxComponent>(TEXT("Ghost Weapon Box"));
	ghost_weapon_box->SetupAttachment(GetRootComponent());
	ghost_weapon_box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ghost_weapon_box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ghost_weapon_box->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);*/

	box_trace_start = CreateDefaultSubobject<USceneComponent>(FName("BoxTraceStart"));
	box_trace_start->SetupAttachment(GetRootComponent());

	box_trace_end = CreateDefaultSubobject<USceneComponent>(FName("BoxTraceEnd"));
	box_trace_end->SetupAttachment(GetRootComponent());

	item_mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	item_mesh->SetSimulatePhysics(true);
	item_mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	item_mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
}

void AWeapon::DisableWeaponBoxCollision()
{
	weapon_box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//ghost_weapon_box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ignored_actors.Empty();
}

void AWeapon::EnableWeaponBoxCollision()
{
	weapon_box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::EnableGhostWeaponBoxCollision()
{
	//ghost_weapon_box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	weapon_box->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	//ghost_weapon_box->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnGhostBoxOverlap);
}

// if collision occurs during the attack, this function is called to check whether damage can be applied
void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	FHitResult box_hit;
	BoxTrace(box_hit);
	if (!box_hit.GetActor()) return;
	if (box_hit.GetActor()->ActorHasTag(FName("Dead"))) return;
	if (GetOwner()->ActorHasTag(FName("Enemy")) && box_hit.GetActor()->ActorHasTag(FName("Enemy"))) return;

	if (IHitInterface* hit_interface = Cast<IHitInterface>(box_hit.GetActor()))
	{
		float damage_multiplier = 1;
		if (ABaseCharacter* instigator = Cast<ABaseCharacter>(box_hit.GetActor())) damage_multiplier = instigator->GetDamageMultiplier();
		UGameplayStatics::ApplyDamage(box_hit.GetActor(), damage * damage_multiplier, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		hit_interface->GetHit(GetOwner(),this, box_hit.ImpactPoint);
		//if (GetWorld()) DrawDebugSphere(GetWorld(), box_hit.ImpactPoint, 25.f, 12, FColor::Red, true);
	}
}

void AWeapon::OnGhostBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if (OtherActor->ActorHasTag(FName("ShieldBox"))) ignored_actors.AddUnique(OtherActor->GetOwner());
}

// box trace
void AWeapon::BoxTrace(FHitResult& box_hit)
{
	FVector start_pos = box_trace_start->GetComponentLocation();
	FVector end_pos = box_trace_end->GetComponentLocation();

	TArray<AActor*> actors_to_ignore;
	actors_to_ignore.Add(this);
	actors_to_ignore.Add(GetOwner());

	for (AActor* actor : ignored_actors)
	{
		actors_to_ignore.AddUnique(actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(this, start_pos, end_pos, box_trace_extent, box_trace_start->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, 
		actors_to_ignore, EDrawDebugTrace::None, box_hit, true);

	ignored_actors.AddUnique(box_hit.GetActor());
}

void AWeapon::GetEquipped(ABaseCharacter* actor)
{
	item_state = EItemState::EIS_Equipped;
	SetEquippedCollision();
	TurnOffUIDisplay();
	SetOwner(actor);
	SetInstigator(actor);
	DisableSphereCollision();
	actor->TakeWeapon(this);
}

void AWeapon::GetUnequipped()
{
	SetItemState(EItemState::EIS_Hovering);
	EnableSphereCollision();
	weapon_box->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	TurnOnUIDisplay();
	FDetachmentTransformRules detach_rules(EDetachmentRule::KeepWorld, true);
	item_mesh->DetachFromComponent(detach_rules);
	SetUnequippedCollision();

}
