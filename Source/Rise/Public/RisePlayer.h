#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "RisePlayer.generated.h"

UCLASS()
class RISE_API ARisePlayer : public APawn
{
	GENERATED_BODY()

public:
	/** The BaseComponent of this pawn. */
	UPROPERTY(EditAnywhere)
	class USceneComponent* SceneComponent;

	/** A MeshComponent for this pawn that is used solely for calculations. */
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComponent;

	/** The camrea spring arm component. */
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* CameraSpringArmComponent;

	/** The camera component. */
	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComponent;

private:

	/** The camera pitch. */
	UPROPERTY(EditDefaultsOnly, Category = "Rise|Camera", meta=(ClampMin="-90", ClampMax="-10"))
	float CameraPitch;

public:
	ARisePlayer();

protected:

	virtual void BeginPlay() override;
};