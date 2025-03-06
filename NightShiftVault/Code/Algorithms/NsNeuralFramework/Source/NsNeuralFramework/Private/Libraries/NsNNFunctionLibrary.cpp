// Copyright (C) 2024 mykaa. All rights reserved.

#include "Libraries/NsNNFunctionLibrary.h"
#include "DesktopPlatformModule.h"
#include "HAL/PlatformApplicationMisc.h"
#include "IDesktopPlatform.h"
#include "NsNNSessionSubsystem.h"
#include "NsNNSettings.h"
#include "NsNNTrainController.h"

FNsNNOnGenotypeImport UNsNNFunctionLibrary::OnGenotypeImport;
FNsNNOnTrainDataImport UNsNNFunctionLibrary::OnTrainDataImport;

FAutoConsoleCommand ExecNsNeuralFrameworkImportTrainDataCmd
(
    TEXT("NsNeuralFramework.ImportTrainData"),
    TEXT("Imports the data from a train session file"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        if (const UNsNNSessionSubsystem* const NeuralSubsystem = UNsNNSessionSubsystem::GetSubsystem())
        {
            if (!NeuralSubsystem->IsTraining())
            {
                TMap<FString, FString> InOutParsedData;
                if (UNsNNFunctionLibrary::LoadAndParseTrainFileData(InOutParsedData, TEXT("=")))
                {
                    const AController* const FoundController = UNsNNFunctionLibrary::GetValueFromTrainData<AController*>(InOutParsedData, TEXT("Controller"));
                    UNsNNFunctionLibrary::OnTrainDataImport.Broadcast(InOutParsedData, FoundController);
                }
            }
        }
    })
);

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
        const FString BasePath = FPaths::Combine(FPaths::ProjectConfigDir(), TEXT("Neural"));
        if (!FPaths::DirectoryExists(BasePath))
        {
            IFileManager::Get().MakeDirectory(*BasePath, true);
        }

        FString FileContents;
        if (!FFileHelper::LoadFileToString(FileContents, *BasePath))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to load file from %s"), *BasePath);
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
            UNsNNFunctionLibrary::OnGenotypeImport.Broadcast(Genotype);
        }
    })
);

AController* UNsNNFunctionLibrary::ChangeControllerForAgent(UClass* const InController, APawn* const InAgent, UWorld* const InWorld)
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

bool UNsNNFunctionLibrary::LoadAndFindValueFromTrainDataFile(const FString& InKey, FString& OutValue, const FString& InSplitDelimiter /**= TEXT("=") */)
{
    TMap<FString, FString> InOutParsedData;
    if (LoadAndParseTrainFileData(InOutParsedData, InSplitDelimiter))
    {
        if (InOutParsedData.Contains(InKey))
        {
            OutValue = InOutParsedData[InKey];
        }
    }

    return true;
}

bool UNsNNFunctionLibrary::LoadAndParseTrainFileData(TMap<FString, FString>& InOutParsedData, const FString& InSplitDelimiter /**= TEXT("=") */)
{
    InOutParsedData.Empty();

    FString FilePath = FPaths::Combine(FPaths::ProjectConfigDir(), TEXT("Neural"));;

    // Open a file dialog to select the file
    if (IDesktopPlatform* const DesktopPlatform = FDesktopPlatformModule::Get())
    {
        const void* const ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
        TArray<FString> OutFiles;
        if (DesktopPlatform->OpenFileDialog(ParentWindowHandle, TEXT("Import Genotype from session file"), !FPaths::DirectoryExists(FilePath) || FilePath.IsEmpty() ? FPaths::ProjectDir() : FilePath, TEXT(""), TEXT("Text Files (*.txt)|*.txt|All Files (*.*)|*.*"), EFileDialogFlags::None, OutFiles))
        {
            FilePath = OutFiles[0];
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("User canceled file selection."));
            return false; // User canceled
        }
    }

    if (FilePath.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("No file selected."));
        return false; // No file selected
    }

    FString FileContents;
    if (!FFileHelper::LoadFileToString(FileContents, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file from %s"), *FilePath);
        return false;
    }

    TArray<FString> Lines;
    FileContents.ParseIntoArrayLines(Lines);
    InOutParsedData.Reserve(Lines.Num());

    // Parse lines into a key-value map
    for (const FString& Line : Lines)
    {
        FString Key, Value;
        if (Line.Split(InSplitDelimiter, &Key, &Value))
        {
            InOutParsedData.Emplace(Key, Value);
        }
    }

    return InOutParsedData.Num() > 0;
}
