// Copyright (C) 2024 mykaa. All rights reserved.

#include "NsNNRuntimeController.h"
#include "Networks/NsNNBaseNetwork.h"

ANsNNRuntimeController::ANsNNRuntimeController()
    : NeuralNetwork(nullptr)
    , InputSize(INDEX_NONE)
    , HiddenLayerSize(INDEX_NONE)
    , OutputSize(INDEX_NONE)
{
    PrimaryActorTick.bCanEverTick = true;
}

void ANsNNRuntimeController::BeginPlay()
{
    Super::BeginPlay();

    ensureMsgf(InputSize != INDEX_NONE || HiddenLayerSize != INDEX_NONE || OutputSize != INDEX_NONE, TEXT("ANsNNRuntimeController::BeginPlay() : Neural Network Sizes were not setup properly, give them proper values"));
    if (NeuralNetworkClass != nullptr)
    {
        NeuralNetwork = NeuralNetworkClass->GetDefaultObject<UNsNNBaseNetwork>();

        if (NeuralNetwork != nullptr)
        {
            const int32 AgentGenotypeSize = NeuralNetwork->Initialize(InputSize, HiddenLayerSize, OutputSize);
            ensureMsgf(AgentGenotypeSize == Genotype.Num(), TEXT("ANsNNRuntimeController::BeginPlay() : Agent Genotype is not properly setup"));
            NeuralNetwork->SetWeights(Genotype);
        }
    }
}

void ANsNNRuntimeController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (NeuralNetwork != nullptr)
    {
        const TArray<float>& Inputs = FeedInputs();
        ensureMsgf(Inputs.Num() == InputSize, TEXT("ANsNNRuntimeController::Tick : Amount of inputs fed do not match with the InputSize that was setup"));
        const TArray<float>& Outputs = NeuralNetwork->ProcessInputs(Inputs);
        HandleOutputs(Outputs);
    }
}

TArray<float> ANsNNRuntimeController::FeedInputs_Implementation()
{
    // Should be overriden by children
    return {};
}

void ANsNNRuntimeController::HandleOutputs_Implementation(const TArray<float>& InOutputs)
{
    // Should be overriden by children
}