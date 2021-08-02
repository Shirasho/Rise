#include "Components/RiseSelectableComponent.h"

#include "Components/DecalComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "RiseMacros.h"

void URiseSelectableComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		return;
	}
	
	DecalComponent = NewObject<UDecalComponent>(Owner, TEXT("SelectedDecal"));
	if (!DecalComponent)
	{
		return;
	}

	//TODO: Get the collision boundaries of the actor.
	float DecalHeight = 0.f;
	float DecalWidth = 0.f;

	// Register the decal.
	DecalComponent->RegisterComponent();
	DecalComponent->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	DecalComponent->DecalSize = FVector(DecalHeight, DecalWidth, DecalWidth);

	// Make the decale face the ground.
	DecalComponent->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0.f, -90.f, 0.f)));

	// Create the decal material.
	if (SelectedMaterial)
	{
		SelectedMaterialInstance = UMaterialInstanceDynamic::Create(SelectedMaterial, this);
		DecalComponent->SetDecalMaterial(SelectedMaterialInstance);
	}
	else
	{
		DEBUG_WARNING(TEXT("SelectableComponent is missing a selected material."));
	}

	// Hide the decal to start.
	DecalComponent->SetHiddenInGame(true);
}


void URiseSelectableComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

	if (IsValid(DecalComponent))
	{
		DecalComponent->DestroyComponent();
	}
}

void URiseSelectableComponent::SelectActor()
{
	if (bSelected)
	{
		return;
	}

	bSelected = true;

	if (IsValid(DecalComponent))
	{
		DecalComponent->SetHiddenInGame(false);
	}

	OnSelected.Broadcast(GetOwner());

	// We are not going to play the audio cue here. If a group of actors is selected
	// we do not want all of them to play their audio at the same time. This would be
	// disastrous with large groups of actors. This logic will be handled elsewhere.
}

void URiseSelectableComponent::DeselectActor()
{
	if (!bSelected)
	{
		return;
	}

	bSelected = false;

	if (IsValid(DecalComponent))
	{
		DecalComponent->SetHiddenInGame(true);
	}

	OnSelected.Broadcast(GetOwner());
}

bool URiseSelectableComponent::IsSelected() const
{
	return bSelected;
}

void URiseSelectableComponent::HoverActor()
{
	if (bHovered)
	{
		return;
	}

	bHovered = true;

	OnHovered.Broadcast(GetOwner());
}

void URiseSelectableComponent::UnhoverActor()
{
	if (!bHovered)
	{
		return;
	}

	bHovered = false;

	OnUnhovered.Broadcast(GetOwner());
}

bool URiseSelectableComponent::IsHovered() const
{
	return bHovered;
}

USoundCue* URiseSelectableComponent::GetSelectedSound() const
{
	return SelectedSound;
}

UTexture2D* URiseSelectableComponent::GetPortrait() const
{
	return Portrait;
}

void URiseSelectableComponent::SetPortrait(UTexture2D* NewPortrait)
{
	Portrait = NewPortrait;
}