#include "Items/Item.h"
#include "Components/SphereComponent.h"
#include "Interfaces/PickupInterface.h"
#include "Characters/BaseCharacter.h"
#include "HUD/ItemUIComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	item_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	//item_mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//item_mesh->SetSimulatePhysics(false);
	//item_mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
	RootComponent = item_mesh;

	sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	sphere->SetupAttachment(GetRootComponent());

	item_ui_widget = CreateDefaultSubobject<UItemUIComponent>("ItemUIWidget");
	item_ui_widget->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	TurnOffUIDisplay();

	if (item_state == EItemState::EIS_Equipped)
	{
		SetEquippedCollision();
	}
	else {
		SetUnequippedCollision();
	}

	if (item_ui_widget) item_ui_widget->SetNameText(item_name);
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* pickup_interface = Cast<IPickupInterface>(OtherActor);
	if (pickup_interface)
	{
		pickup_interface->SetOverlappingItem(this);
		if (GEngine) GEngine->AddOnScreenDebugMessage(4, 3, FColor::Red, FString("Start overlap"));
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickupInterface* pickup_interface = Cast<IPickupInterface>(OtherActor);
	if (pickup_interface) 
	{
		pickup_interface->RemoveOverlappingItem(this);
		if (GEngine) GEngine->AddOnScreenDebugMessage(6, 3, FColor::Red, FString("End overlap"));
	}
}

void AItem::GetEquipped(ABaseCharacter* actor)
{
	Destroy();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	running_time += DeltaTime;

	if (item_state == EItemState::EIS_Hovering)
	{
		float deltaz = amplitude * FMath::Sin(running_time * time_constant);

		AddActorWorldOffset(FVector(0, 0, deltaz));
	}
}

void AItem::DisableSphereCollision()
{
	if (sphere)
	{
		sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AItem::EnableSphereCollision()
{
	if (sphere) sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AItem::AttachMeshToSocket(USceneComponent* parent_mesh, FName socket_name)
{
	FAttachmentTransformRules transform_rules(EAttachmentRule::SnapToTarget, true);
	item_mesh->AttachToComponent(parent_mesh, transform_rules, socket_name);
}

void AItem::TurnOnUIDisplay()
{
	if (item_ui_widget) item_ui_widget->SetVisibility(true);
}

void AItem::TurnOffUIDisplay()
{
	if (item_ui_widget) item_ui_widget->SetVisibility(false);
}

void AItem::SetUnequippedCollision()
{
	item_mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	item_mesh->SetSimulatePhysics(true);
	item_mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	item_mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
}

void AItem::SetEquippedCollision()
{
	item_mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	item_mesh->SetSimulatePhysics(false);
	item_mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}
