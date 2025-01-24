// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NsNNTypeLibrary.h"
#include "NsNNFunctionLibrary.generated.h"

/** Notifier for when the genotype has been imported */
DECLARE_MULTICAST_DELEGATE_OneParam(FNsNNOnGenotypeImport, const TArray<float>& InGenotype);

/** Notifier for when a train data file has been imported */
typedef TMap<FString, FString> FNsNNParsedData;
DECLARE_MULTICAST_DELEGATE_TwoParams(FNsNNOnTrainDataImport, const FNsNNParsedData& InParsedData, const AController* const InUsedController);

/**
 * Neural Network Function Library
 */
UCLASS()
class NSNEURALFRAMEWORK_API UNsNNFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

// Functions
public:

    /** Whether we have a specific flag active */
    template<typename T>
    static bool HasActiveFlag(const uint8 InFlags, T InFlagToCheck)
    {
        return InFlags & static_cast<uint8>(InFlagToCheck);
    }

    /** Activate specific flag */
    template<typename T>
    static void ActivateFlag(uint8& InFlags, const T InNewState)
    {
        InFlags |= static_cast<uint8>(InNewState);
    }

    /** Spawn AI from class */
    UFUNCTION()
    static class APawn* SpawnAndPocessAgent(const class UObject* const InWorldContextObject, class UClass* const InPawnClass, class UClass* const InController, const FVector& InLocation, const FRotator& InRotation, class AActor* const InOwner);

    /** Spawn controller in agent */
    UFUNCTION()
    static class AController* ChangeControllerForAgent(class UClass* const InController, class APawn* const InAgent, class UWorld* const InWorld);

    /** Compress Genotype for better readability and storage */
    UFUNCTION()
    static FString CompressGenotype(const TArray<float>& InGenotype);

    /** Decode Genotype */
    UFUNCTION()
    static TArray<float> DecompressGenotype(const FString& InGenotypeData);

    /** Get Session Data From Handle */
    UFUNCTION(BlueprintCallable)
    static FNsTrainSessionSetup GetSessionDataFromRow(const FNsTrainSessionRowHandle& InRowHandle);

    /** Loads and parses the data from a train file */
    UFUNCTION(BlueprintCallable)
    static bool LoadAndFindValueFromTrainDataFile(const FString& InKey, FString& OutValue, const FString& InSplitDelimiter);
    static bool LoadAndParseTrainFileData(TMap<FString, FString>& InOutParsedData, const FString& InSplitDelimiter = TEXT("="));

    /** Get value from parsed train data */
    template <typename T>
    static T GetValueFromTrainData(const TMap<FString, FString>& InParsedTrainData, const FString& InKey, const FString& InSplitDelimiter = TEXT("="))
    {
        if (InParsedTrainData.Contains(InKey))
        {
            using DecayedPtr = std::remove_pointer_t<std::decay_t<T>>;
            //// Handle float type
            if constexpr (std::is_same_v<T, float>)
            {
                return FCString::Atof(*InParsedTrainData[InKey]);
            }
            // Handle int32 type
            else if constexpr (std::is_same_v<T, int32>)
            {
                return FCString::Atoi(*InParsedTrainData[InKey]);
            }
            // Handle FString type
            else if constexpr (std::is_same_v<T, FString>)
            {
                return InParsedTrainData[InKey];
            }
            // Handle UObject-derived types, including TObjectPtr
            else if constexpr (std::is_base_of_v<UObject, DecayedPtr>)
            {
                const FSoftObjectPath SoftObjectPath(InParsedTrainData[InKey]);
                const TSoftClassPtr<DecayedPtr> ObjectClass(SoftObjectPath);
                if (ObjectClass.IsValid())
                {
                    const UClass* const LoadedObject = ObjectClass.LoadSynchronous();
                    if (LoadedObject != nullptr)
                    {
                        return NewObject<DecayedPtr>(GEngine, LoadedObject);
                    }
                }
            }
        }
        return T{};
    }

// Events
public:

    /** On Genotype import */
    static FNsNNOnGenotypeImport OnGenotypeImport;

    /** On Train Data import */
    static FNsNNOnTrainDataImport OnTrainDataImport;
};