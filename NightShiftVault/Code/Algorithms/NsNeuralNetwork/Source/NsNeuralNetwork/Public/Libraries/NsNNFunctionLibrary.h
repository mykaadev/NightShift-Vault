// Copyright © 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NsNNTypeLibrary.h"
#include "NsNNFunctionLibrary.generated.h"

/**
 * Neural Network Function Library
 */
UCLASS()
class NSNEURALNETWORK_API UNsNNFunctionLibrary : public UBlueprintFunctionLibrary
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

    /** Spawn AI From Class */
    UFUNCTION()
    static class APawn* SpawnAgentFromClass(const UObject* const InWorldContextObject, const TSubclassOf<APawn> InPawnClass, const TSubclassOf<AController> InController, const FVector& Location, const FRotator& Rotation, AActor* const Owner);

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