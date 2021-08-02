#include "Components/RiseActorComponent.h"

void URiseActorComponent::AddGameplayTags(FGameplayTagContainer& TagContainer)
{
	TagContainer.AppendTags(InitialGameplayTags);
}