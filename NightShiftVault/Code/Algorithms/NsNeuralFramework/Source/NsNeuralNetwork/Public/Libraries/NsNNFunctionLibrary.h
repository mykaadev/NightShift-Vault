// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NsNNTypeLibrary.h"
#include "NsNNFunctionLibrary.generated.h"

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
    static class APawn* SpawnAgent(const class UObject* const InWorldContextObject, class UClass* const InPawnClass, class UClass* const InController, const FVector& InLocation, const FRotator& InRotation, class AActor* const InOwner);

    /** Spawn controller in agent */
    UFUNCTION()
    static class AController* SpawnControllerInAgent(class UClass* const InController, class APawn* const InAgent, class UWorld* const InWorld);

    /** Compress Genotype for better readability and storage */
    UFUNCTION()
    static FString CompressGenotype(const TArray<float>& InGenotype);

    /** Decode Genotype */
    UFUNCTION()
    static TArray<float> DecompressGenotype(const FString& InGenotypeData);

    /** Get Session Data From Handle */
    UFUNCTION(BlueprintCallable)
    static FNsTrainSessionSetup GetSessionDataFromRow(const FNsTrainSessionRowHandle& InRowHandle);
};