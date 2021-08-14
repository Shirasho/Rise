#include "WorldGen/WorldGenerator.h"

int32 FWorldGenerator::GetWidth() const
{
	return Width;
}

int32 FWorldGenerator::GetDepth() const
{
	return Depth;
}

float FWorldGenerator::GetFrequency() const
{
	return Frequency;
}

int32 FWorldGenerator::GetOctaves() const
{
	return Octaves;
}

float FWorldGenerator::GetRedistribution() const
{
	return Redistribution;
}

float FWorldGenerator::GetValue(int32 x, int32 y) const
{
	check(x >= 0 && x < Width);
	check(y >= 0 && y < Depth);
	
	return GetValues()[y * Width + x];
}

const TArray<float>& FWorldGenerator::GetValues() const
{
	return Data;
}

void FWorldGenerator::Generate()
{
	if (!Data.IsEmpty())
	{
		return;
	}
	
	for (int32 y = 0; y < Depth; ++y)
	{
		for (int32 x = 0; x < Width; ++x)
		{
			float vx = Frequency * (x / Width - 0.5f);
			float vy = Frequency * (y / Depth - 0.5f);
			
			float Noise = 0;
			float AmplitudeSum = 0;
			for (int32 O = 1; O <= Octaves; ++O)
			{
				float Amplitude = 1/O;
				float Modifier = FMath::Pow(2.f, O-1);
				
				// Add some variance so the X and Y values are independent.
				float OctaveAdjustmentX = FMath::Pow(0.81f, static_cast<float>(O)) + 0.69f/O;
				float OctaveAdjustmentY = FMath::Pow(0.5f, static_cast<float>(O)) + 0.7f/O;
				
				Noise += Amplitude + GenerateNoise(vx * Modifier + OctaveAdjustmentX, vy * Modifier + OctaveAdjustmentY);
				AmplitudeSum += Amplitude;
			}
			
			// Normalize the amplitudes.
			Noise /= AmplitudeSum;
			
			Noise = FMath::Pow(Noise, Redistribution); 
			
			Data.Add(Noise);
		}
	}
}

float FWorldGenerator::GenerateNoise(float x, float y)
{
	return FMath::PerlinNoise2D(FVector2D(x, y));
}