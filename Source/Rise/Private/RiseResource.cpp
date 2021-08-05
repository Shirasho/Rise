#include "RiseResource.h"

#include "Engine/Texture2D.h"

#include "RiseMacros.h"

FText URiseResource::GetResourceName() const
{
	return ResourceName;
}

UTexture2D* URiseResource::GetResourceIcon() const
{
	return ResourceIcon;
}

FGameplayTagContainer URiseResource::GetResourceTags() const
{
	return ResourceTags;
}