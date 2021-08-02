#pragma once

#include "CoreMinimal.h"
#include "Structures/RiseStructure.h"
#include "RiseCommandStation.generated.h"

UCLASS()
class RISE_API ARiseCommandStation : public ARiseStructure
{
	GENERATED_BODY()
	
protected:

	virtual const TCHAR* GetDataSetKey() const override;
};
