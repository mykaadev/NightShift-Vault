// Copyright (C) 2024 mykaa. All rights reserved.

#include "Libraries/NsNNFunctionLibrary.h"
#include "DesktopPlatformModule.h"
#include "HAL/PlatformApplicationMisc.h"
#include "IDesktopPlatform.h"
#include "NsNNSessionSubsystem.h"
#include "NsNNSettings.h"

FAutoConsoleCommand ExecNsNeuralFrameworkStartTrainSessionCmd
(
    TEXT("NsNeuralFramework.StartTrainSession"),
    TEXT("Start a train session"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        if (UNsNNSessionSubsystem* const NeuralSubsystem = UNsNNSessionSubsystem::GetSubsystem())
        {
            if (NeuralSubsystem->GetControlPanel() == nullptr)
            {
                NeuralSubsystem->Init();
            }
        }
    })
);

FAutoConsoleCommand ExecNsNeuralFrameworkImportGenotypeFromFileCmd
(
    TEXT("NsNeuralFramework.ImportGenotype"),
    TEXT("Imports a genotype from a file to the user clipboard"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        FString FilePath;

        // Open a file dialog to select the file
        if (const UNsNNSettings* const Settings = GetDefault<UNsNNSettings>())
        {
            const FString BasePath = Settings->DataExportPath;
            if (IDesktopPlatform* const DesktopPlatform = FDesktopPlatformModule::Get())
            {
                const void* const ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
                TArray<FString> OutFiles;
                if (DesktopPlatform->OpenFileDialog(ParentWindowHandle, TEXT("Import Genotype from session file"), !FPaths::DirectoryExists(BasePath) || BasePath.IsEmpty() ? FPaths::ProjectDir() : BasePath, TEXT(""), TEXT("Text Files (*.txt)|*.txt|All Files (*.*)|*.*"), EFileDialogFlags::None, OutFiles))
                {
                    FilePath = OutFiles[0];
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("User canceled file selection."));
                    return; // User canceled
                }
            }
        }

        if (FilePath.IsEmpty())
        {
            UE_LOG(LogTemp, Error, TEXT("No file selected."));
            return; // No file selected
        }

        FString FileContents;
        if (!FFileHelper::LoadFileToString(FileContents, *FilePath))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to load file from %s"), *FilePath);
            return;
        }

        TMap<FString, FString> ParsedData;
        TArray<FString> Lines;
        FileContents.ParseIntoArrayLines(Lines);

        // Parse lines into a key-value map
        for (const FString& Line : Lines)
        {
            FString Key, Value;
            if (Line.Split(TEXT("="), &Key, &Value))
            {
                ParsedData.Emplace(Key, Value);
            }
        }

        auto CopyArrayToClipboard = [](const TArray<float>& InFloatArray) -> FString
        {
            FString Genotype;
            for (int32 i = 0; i < InFloatArray.Num(); ++i)
            {
                Genotype += FString::SanitizeFloat(InFloatArray[i]);
                if (i < InFloatArray.Num() - 1)
                {
                    Genotype += TEXT(",");
                }
            }
            const FString ClipboardContent = TEXT("(") + Genotype + TEXT(")");
            FPlatformApplicationMisc::ClipboardCopy(*ClipboardContent);
            return ClipboardContent;
        };

        const FString Key(TEXT("Genotype"));
        if (ParsedData.Contains(Key))
        {
            const TArray<float>& Genotype = UNsNNFunctionLibrary::DecompressGenotype(ParsedData[Key]);
            CopyArrayToClipboard(Genotype);
        }
    })
);

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
