#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "RiseSelectableComponent.generated.h"

class UMaterialInterface;
class UMaterialInstanceDynamic;
class USoundCue;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectableComponentSelectedSignature, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectableComponentDeselectedSignature, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectableComponentHoveredSignature, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectableComponentUnhoveredSignature, AActor*, Actor);

/**
 * When attached to an actor, allows this actor to be selected by a player.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RISE_API URiseSelectableComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	/** Event called when the owning actor has been selected. */
	UPROPERTY(BlueprintAssignable, Category = "Rise")
	FSelectableComponentSelectedSignature OnSelected;

	/** Event called when the owning actor has been deselected. */
	UPROPERTY(BlueprintAssignable, Category = "Rise")
	FSelectableComponentDeselectedSignature OnDeselected;

	/** Event called when the local player hovers over the owning actor. */
	UPROPERTY(BlueprintAssignable, Category = "Rise")
	FSelectableComponentHoveredSignature OnHovered;

	/** Event called when the local player unhovers from the owning actor. */
	UPROPERTY(BlueprintAssignable, Category = "Rise")
	FSelectableComponentUnhoveredSignature OnUnhovered;

private:

	/** The material for the selection decal of the owning actor. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	UMaterialInterface* SelectedMaterial;

	/** The sound to play when the owning actor is selected. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	USoundCue* SelectedSound;

	/** The portrait to display in the UI when the owning actor is selected. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise")
	UTexture2D* Portrait;

	/** Whether the owning actor is currently selected by the local player. */
	bool bSelected;

	/** Whether the owning actor is currently being hovered over by the local player. */
	bool bHovered;

	/** The decal component to display when the owning actor is selected. */
	UPROPERTY()
	UDecalComponent* DecalComponent;

	/** The material instance for rendering the selection decal. */
	UPROPERTY()
	UMaterialInstanceDynamic* SelectedMaterialInstance;

protected:

	virtual void BeginPlay() override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;

public:

	/**
	 * Selects the owning actor for the local player.
	 */
	UFUNCTION(BlueprintCallable)
	void SelectActor();

	/**
	 * Deselects the owning actor for the local player.
	 */
	UFUNCTION(BlueprintCallable)
	void DeselectActor();

	/**
	 * Checks whether the owning actor is currently selected by the local player.
	 * 
	 * @return Whether the owning actor is currently selected by the local player.
	 */
	UFUNCTION(BlueprintPure)
	bool IsSelected() const;

	/*
	 * Hovers over the owning actor.
	 */
	void HoverActor();

	/**
	 * Unhovers off the owning actor.
	 */
	void UnhoverActor();

	/**
	 * Checks whether the owning actor is being hovered over (by the mouse for example) by the local player.
	 * 
	 * @return Whether the owning actor is being hovered over (by the mouse for example) by the local player.
	 */
	UFUNCTION(BlueprintPure)
	bool IsHovered() const;

	/**
	 * Returns the sound to play when the owning actor is selected.
	 * 
	 * @return The sound queue to play when the owning actor is selected.
	 */
	USoundCue* GetSelectedSound() const;

	/**
	 * Returns the texture to display in the UI when the owning actor is selected.
	 * 
	 * @return The texture to display in the UI when the owning actor is selected.
	 */
	UFUNCTION(BlueprintPure, Category = "Rise")
	UTexture2D* GetPortrait() const;

	/**
	 * Sets the texture to display in the UI when the owning actor is selected.
	 * 
	 * @param NewPortrait The new texture to display in the UI when the owning actor is selected.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rise")
	void SetPortrait(UTexture2D* NewPortrait);
};
