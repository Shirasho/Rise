#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SoftObjectPtr.h"

#include "RiseMacros.h"
#include "Components/Metadata/RiseNameComponent.h"

/**
 * A structure that contains multiple utilities that would not work as macros.
 */
struct RISE_API RiseHelpers
{
public:

	FText GetName(const AActor* Actor)
	{
		if (!Actor)
		{
			return FText::FromString(TEXT(""));
		}

		URiseNameComponent* NameComponent = Actor->FindComponentByClass<URiseNameComponent>();
		if (NameComponent)
		{
			return NameComponent->GetName();
		}

		return FText::FromString(Actor->GetName());
	}


	/**
	 * Finds blueprint classes in both editor and cooked packages by content path.
	 * The resulting UClass references should be cached for reuse as this operation
	 * is very expensive.
	 */
	template<class TBaseClass>
	struct FBlueprintClassFinder
	{
		TSoftClassPtr<TBaseClass> Class;
		TArray<TSoftClassPtr<TBaseClass>> Classes;

		FBlueprintClassFinder(const TCHAR* ContentPath, const TCHAR* ClassNameHint = NULL, const TCHAR* AssetRegistryModule = TEXT("AssetRegistry"))
		{
			IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryModule).Get();

			// The asset registry contents may not be in memory at this point.
			// Scan them just to be safe.
			TArray<FString> ContentPaths;
			ContentPaths.Add(FString(ContentPath));
			AssetRegistry.ScanPathsSynchronous(ContentPaths);

			FName BaseClassName = TBaseClass::StaticClass()->GetFName();
			TSet<FName> DerivedNames;
			{
				TArray<FName> BaseNames;
				BaseNames.Add(BaseClassName);

				TSet<FName> Excluded;
				AssetRegistry.GetDerivedClassNames(BaseNames, Excluded, DerivedNames);
			}

			// Create the asset registry filter. We are looking specifically for UBlueprints.
			FARFilter Filter;
			Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
			Filter.bRecursiveClasses = true;
			Filter.bRecursivePaths = true;

			// Get a list of UBlueprint assets.
			TArray<FAssetData> AssetList;
			AssetRegistry.GetAssets(Filter, AssetList);

			// For each asset in the asset list...
			for (auto const& Asset : AssetList)
			{
				// Determine whether the asset is a generated class.
				FString GeneratedClassPathPtr = Asset.TagsAndValues.FindTag(TEXT("GeneratedClass")).AsString();
				if (!GeneratedClassPathPtr.IsEmpty())
				{
					// The class name and object path differs between editor and cooked builds.
					// This will allow us to get the correct class name regardless of where this method
					// is called. This is essentially work that is done in AssetRegistry.GetDerivedClassNames()
					// which was called above.
					const FString ClassObjectPath = FPackageName::ExportTextPathToObjectPath(*GeneratedClassPathPtr);
					const FString ClassName = FPackageName::ObjectPathToObjectName(ClassObjectPath);

					// If this class is not a class we are looking for, skip it.
					if (!DerivedNames.Contains(*ClassName))
					{
						continue;
					}

					FSoftObjectPath SoftObjectPath(ClassObjectPath);
					if (ClassNameHint == NULL || SoftObjectPath.GetAssetName().Contains(ClassNameHint))
					{
						Classes.Add(TSoftClassPtr<TBaseClass>(SoftObjectPath));
					}
				}
			}

			if (!Classes.IsEmpty())
			{
				Class = Classes[0];
			}
		}

		/**
		 * Whether at least one blueprint class was found.
		 */
		bool Succeeded()
		{
			return Class != NULL;
		}
	};
};