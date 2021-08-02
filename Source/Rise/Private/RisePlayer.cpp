#include "RisePlayer.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "RiseMacros.h"

// https://www.youtube.com/watch?v=MpKXKr9alqY
// https://forums.unrealengine.com/t/how-can-i-create-a-timelinecomponent-in-c/8541/9
// https://docs.unrealengine.com/4.26/en-US/ProgrammingAndScripting/ProgrammingWithCPP/CPPTutorials/PlayerCamera/

//#define BIND_CAMERA_MOVEMENT_TIMELINE_FUNCS(Direction) \
//	FOnTimelineFloat Cam##Direction##Interp{}; \
//	FOnTimelineEvent Cam##Direction##Finished{}; \
//	Cam##Direction##Interp.BindUFunction(this, MAKE_NAME(OnMoveCamera##Direction##Update)); \
//	Cam##Direction##Finished.BindUFunction(this, MAKE_NAME(OnMoveCamera##Direction##Finished)); \
//	CameraMovement##Direction##Timeline.AddInterpFloat(CameraMovementCurve, Cam##Direction##Interp, FName("Delta")); \
//	CameraMovement##Direction##Timeline.SetTimelineFinishedFunc(Cam##Direction##Finished); \
//	CameraMovement##Direction##Timeline.SetLooping(false); \

//#define DEFINE_CAMERA_MOVEMENT_FUNCS(Direction) \
//	void AGamePlayer::OnMoveCamera##Direction##Finished(){ } \
//	void AGamePlayer::MoveCamera##Direction##Start() \
//	{ \
//		DEBUG_INFO(TEXT("Button pressed")); \
//		if (CameraMovementCurve) \
//		{ \
//			DEBUG_INFO(TEXT("Playing Timeline")); \
//			CameraMovement##Direction##Timeline.PlayFromStart(); \
//		} \
//    } \
//	void AGamePlayer::MoveCamera##Direction##Stop() \
//	{ \
//		if (CameraMovementCurve && CameraMovement##Direction##Timeline.GetPlaybackPosition() != 0) \
//		{ \
//			CameraMovement##Direction##Timeline.Reverse(); \
//		} \
//	} \
//	void AGamePlayer::OnMoveCamera##Direction##Update(float Value) \

// Sets default values
ARisePlayer::ARisePlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
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