#include "RisePlayer.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "RiseMacros.h"

ARisePlayer::ARisePlayer()
{
	PrimaryActorTick.bCanEverTick = false;

	CameraPitch = -55.f;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	// Rotate the entire actor 45 degrees. This will make it easier to work with camera movement and
	// forward/right vectors.
	SceneComponent->SetWorldRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, 45.f)));
	RootComponent = SceneComponent;
		
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraFocalPoint"));
	MeshComponent->SetMobility(EComponentMobility::Movable);
	MeshComponent->SetupAttachment(SceneComponent);

	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArmComponent->SetupAttachment(MeshComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(CameraSpringArmComponent);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ARisePlayer::BeginPlay()
{
	Super::BeginPlay();

	// Rotate the spring arm to make it look down towards the terrain.
	CameraSpringArmComponent->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0.f, CameraPitch, 0.f)));
}