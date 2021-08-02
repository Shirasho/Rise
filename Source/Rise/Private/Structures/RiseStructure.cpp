#include "Structures/RiseStructure.h"
#include "Components/ShapeComponent.h"
#include "Data/RiseStructureData.h"
#include "RiseMacros.h"

ARiseStructure::ARiseStructure()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> StatsDataTableObject(TEXT("DataTable'/Game/Data/Tables/StructuresDataTable'"));
	if (StatsDataTableObject.Succeeded())
	{
		StatsDataTable = StatsDataTableObject.Object;
	}
	else
	{
		DEBUG_ERROR(TEXT("Unable to find StructuresDataTable."));
	}
}