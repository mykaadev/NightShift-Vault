// Copyright (C) 2024 mykaa. All rights reserved.

#include "NsNNRuntimeController.h"
#include "Networks/NsNNArchitecture.h"
#include "NsNNSessionSubsystem.h"

ANsNNRuntimeController::ANsNNRuntimeController()
    : NeuralNetwork(nullptr)
    , InputSize(INDEX_NONE)
    , HiddenLayerSize(INDEX_NONE)
    , OutputSize(INDEX_NONE)
{
    PrimaryActorTick.bCanEverTick = true;
}

ANsNNRuntimeController::~ANsNNRuntimeController()
{
    UNsNNFunctionLibrary::OnTrainDataImport.RemoveAll(this);
}

void ANsNNRuntimeController::BeginPlay()
{
    Super::BeginPlay();
    UNsNNFunctionLibrary::OnTrainDataImport.AddUObject(this, &ANsNNRuntimeController::OnTrainDataImported);
}

void ANsNNRuntimeController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (NeuralNetwork != nullptr)
    {
        StoredInputs = FeedInputs();
        StoredPredictedOutputs = NeuralNetwork->ProcessInformation(StoredInputs, this);

        // Handle the outputs
        HandlePredictedOutputs(StoredPredictedOutputs);
    }
}

void ANsNNRuntimeController::Initialize_Implementation()
{
    if (const UNsNNSessionSubsystem* const NeuralTrainSubsystem = UNsNNSessionSubsystem::GetSubsystem())
    {
        if (NeuralNetwork != nullptr && !NeuralTrainSubsystem->IsTraining())
        {
            NeuralNetwork->InitializeNetwork(InputSize, HiddenLayerSize, OutputSize);
            NeuralNetwork->InitializeRegulators(0.f, 0.f, 0.f); // Runtime controllers are not regulated :) free spirit rules here
        }
    }
}

TArray<float> ANsNNRuntimeController::FeedInputs_Implementation()
{
    // Should be overriden by children
    return {};
}

TArray<float> ANsNNRuntimeController::FeedExpectedOutputs_Implementation()
{
    // Should be overriden by children
    return {};
}

void ANsNNRuntimeController::HandlePredictedOutputs_Implementation(const TArray<float>& InOutputs)
{
    // Should be overriden by children
}

void ANsNNRuntimeController::OnTrainDataImported(const TMap<FString, FString>& InParsedData, const AController* const InController)
{
    if (InController != nullptr && InController->GetClass() == GetClass())
    {
        NeuralNetwork = UNsNNFunctionLibrary::GetValueFromTrainData<UNsNNArchitecture*>(InParsedData, TEXT("NeuralNetwork"));

        InputSize = UNsNNFunctionLibrary::GetValueFromTrainData<int32>(InParsedData, TEXT("NeuralInputs"));
        HiddenLayerSize = UNsNNFunctionLibrary::GetValueFromTrainData<int32>(InParsedData, TEXT("NeuralHiddenLayerSize"));
        OutputSize = UNsNNFunctionLibrary::GetValueFromTrainData<int32>(InParsedData, TEXT("NeuralOutputs"));

        Genotype = UNsNNFunctionLibrary::DecompressGenotype(UNsNNFunctionLibrary::GetValueFromTrainData<FString>(InParsedData, TEXT("Genotype")));

        Initialize_Implementation();
    }
}
