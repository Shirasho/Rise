#include "Components/Metadata/RiseDescriptionComponent.h"

FText URiseDescriptionComponent::GetDescription() const
{
	return Description;
}

void URiseDescriptionComponent::SetDescription(const FText& NewDescription)
{
	Description = NewDescription;
}