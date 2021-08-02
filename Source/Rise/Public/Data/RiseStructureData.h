#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "RiseStructureData.generated.h"

USTRUCT(BlueprintType)
struct FRiseStructureData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FRiseStructureData()
		: Health(100)
		, Armor(0)
	{}

	/**
	 * The maximum health of the structure.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Health;

	/**
	 * The armor of the structure.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Armor;
};