// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RiseStructure.generated.h"

UCLASS(Abstract)
class RISE_API ARiseStructure : public AActor
{
	GENERATED_BODY()

protected:

	/**
	 * The data table that contains the structure property data.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UDataTable* StatsDataTable;

public:
	ARiseStructure();

protected:

	/**
	 * Returns the data set lookup key for this entity.
	 */
	virtual const TCHAR* GetDataSetKey() const PURE_VIRTUAL(AStructure::GetDataSetKey, return TEXT("");)
};
