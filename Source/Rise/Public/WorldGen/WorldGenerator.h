#pragma once

#include "CoreMinimal.h"

// https://www.redblobgames.com/maps/terrain-from-noise/

/**
 * A structure for generating a heightmap using perlin noise.
 */
struct RISE_API FWorldGenerator
{
private:
	int32 Width;
	int32 Depth;
	float Frequency;
	int32 Octaves;
	float Redistribution;
	TArray<float> Data;

public:

	FWorldGenerator(int32 InWidth, int32 InDepth, float InFrequency)
		: Width(InWidth)
		, Depth(InDepth)
		, Frequency(InFrequency)
		, Octaves(3)
		, Redistribution(1)
	{

	}

	FWorldGenerator(int32 InWidth, int32 InDepth, float InFrequency, int32 InOctaves, float InRedistribution)
		: Width(InWidth)
		, Depth(InDepth)
		, Frequency(InFrequency)
		, Octaves(InOctaves)
		, Redistribution(InRedistribution)
	{

	}

	int32 GetWidth() const;
	int32 GetDepth() const;
	float GetFrequency() const;
	int32 GetOctaves() const;
	float GetRedistribution() const;
	float GetValue(int32 x, int32 y) const;
	const TArray<float>& GetValues() const;

private:
	void Generate();
	float GenerateNoise(float x, float y);
};