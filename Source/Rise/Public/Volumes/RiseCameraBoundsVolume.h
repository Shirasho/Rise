#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"

#include "RiseCameraBoundsVolume.generated.h"

/**
 * A volume that restricts the camera movement. There should only be one per level.
 */
UCLASS()
class RISE_API ARiseCameraBoundsVolume : public AVolume
{
	GENERATED_BODY()
};
