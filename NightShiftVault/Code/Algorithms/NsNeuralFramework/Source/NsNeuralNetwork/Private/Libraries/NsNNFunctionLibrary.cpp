// Copyright (C) 2024 mykaa. All rights reserved.

#include "Libraries/NsNNFunctionLibrary.h"

APawn* UNsNNFunctionLibrary::SpawnAgentFromClass(const UObject* const InWorldContextObject, UClass* const InPawnClass, UClass* const InController, const FVector& Location, const FRotator& Rotation, AActor* const Owner)
{
    APawn* Agent = nullptr;
    if (GEngine != nullptr)
    {
        UWorld* const World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
        if (World != nullptr && InPawnClass != nullptr)
        {
            FActorSpawnParameters ActorSpawnParams;
            ActorSpawnParams.Owner = Owner;
            ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            Agent = World->SpawnActor<APawn>(InPawnClass, Location, Rotation, ActorSpawnParams);

            if (Agent != nullptr && Agent->Controller == nullptr && InController != nullptr)
            {
                FActorSpawnParameters SpawnInfo;
                SpawnInfo.Instigator = Agent->GetInstigator();
                SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                SpawnInfo.OverrideLevel = Agent->GetLevel();
                SpawnInfo.ObjectFlags |= RF_Transient; // We never want to save AI controllers into a map

                if (AController* const NewController = World->SpawnActor<AController>(InController, Agent->GetActorLocation(), Agent->GetActorRotation(), SpawnInfo))
                {
                    // If successful will result in setting Agent->Controller as part of possession mechanics
                    NewController->Possess(Agent);
                }
            }
        }
    }

    return Agent;
}

FString UNsNNFunctionLibrary::CompressGenotype(const TArray<float>& InGenotype)
{
    TArray<int32> ScaledValues;
    for (const float Value : InGenotype)
    {
        // Scale the float by 10000 and convert to an integer
        ScaledValues.Emplace(FMath::RoundToInt(Value * 10000));
    }

    // Convert the array of scaled values to a space-separated string
    const FString CompressedString = FString::JoinBy(ScaledValues, TEXT("|"), [] (const int32 ScaledValue)
    {
        return FString::Printf(TEXT("%d"), ScaledValue);
    });

    return CompressedString;
}

TArray<float> UNsNNFunctionLibrary::DecompressGenotype(const FString& InGenotypeData)
{
    TArray<float> DecodedFloats;
    TArray<FString> StringValues;

    // Split the string into individual components (assuming space separation)
    InGenotypeData.ParseIntoArray(StringValues, TEXT("|"), true);

    for (const FString& StrValue : StringValues)
    {
        // Convert each value back to an integer, then divide by 10000 to get the original float
        const int32 ScaledValue = FCString::Atoi(*StrValue);
        DecodedFloats.Emplace(ScaledValue * 0.0001f);
    }

    return DecodedFloats;
}

FNsTrainSessionSetup UNsNNFunctionLibrary::GetSessionDataFromRow(const FNsTrainSessionRowHandle& InRowHandle)
{
    return InRowHandle.SessionSetup;
}
