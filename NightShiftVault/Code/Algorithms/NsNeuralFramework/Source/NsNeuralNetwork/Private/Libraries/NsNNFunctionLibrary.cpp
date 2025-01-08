// Copyright (C) 2024 mykaa. All rights reserved.

#include "Libraries/NsNNFunctionLibrary.h"

APawn* UNsNNFunctionLibrary::SpawnAgent(const UObject* const InWorldContextObject, UClass* const InPawnClass, UClass* const InController, const FVector& InLocation, const FRotator& InRotation, AActor* const InOwner)
{
    APawn* Agent = nullptr;
    if (GEngine != nullptr)
    {
        UWorld* const World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
        if (World != nullptr && InPawnClass != nullptr)
        {
            FActorSpawnParameters ActorSpawnParams;
            ActorSpawnParams.Owner = InOwner;
            ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            Agent = World->SpawnActor<APawn>(InPawnClass, InLocation, InRotation, ActorSpawnParams);
            SpawnControllerInAgent(InController, Agent, World);
        }
    }

    return Agent;
}

AController* UNsNNFunctionLibrary::SpawnControllerInAgent(UClass* const InController, APawn* const InAgent, UWorld* const InWorld)
{
    AController* Controller = nullptr;
    if (InWorld != nullptr && InAgent != nullptr && InController != nullptr)
    {
        FActorSpawnParameters SpawnInfo;
        SpawnInfo.Instigator = InAgent->GetInstigator();
        SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnInfo.OverrideLevel = InAgent->GetLevel();
        SpawnInfo.ObjectFlags |= RF_Transient; // We never want to save AI controllers into a map

        Controller = InWorld->SpawnActor<AController>(InController, InAgent->GetActorLocation(), InAgent->GetActorRotation(), SpawnInfo);
        if (Controller != nullptr)
        {
            // If successful will result in setting InAgent->Controller as part of possession mechanics
            Controller->Possess(InAgent);
        }
    }

    return Controller;
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
