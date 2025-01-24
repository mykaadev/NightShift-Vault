// Copyright (C) 2024 mykaa. All rights reserved.

#include "Networks/NsNNTwoLayerFF.h"
#include "Kismet/KismetMathLibrary.h"
#include "NsNNSessionSubsystem.h"

UNsNNTwoLayerFF::UNsNNTwoLayerFF()
    : InputLayerSize(INDEX_NONE)
    , HiddenLayerSize(INDEX_NONE)
    , OutputLayerSize(INDEX_NONE)
{}

int32 UNsNNTwoLayerFF::InitializeNetwork(const int32 InInputs, const int32 InHiddenLayer, const int32 InOutputs)
{
    InputLayerSize = InInputs;
    HiddenLayerSize = InHiddenLayer;
    OutputLayerSize = InOutputs;

    ensureMsgf(InputLayerSize != INDEX_NONE || HiddenLayerSize != INDEX_NONE || OutputLayerSize != INDEX_NONE, TEXT("%s : Network Layers sizes are not properly setup"), ANSI_TO_TCHAR(__func__));

    WeightGroupOne.SetNum((InputLayerSize + 1) * HiddenLayerSize);
    WeightGroupTwo.SetNum((HiddenLayerSize + 1) * OutputLayerSize);

    HiddenLayerOutputs.SetNum(HiddenLayerSize);
    OutputLayerOutputs.SetNum(OutputLayerSize);

    return WeightGroupOne.Num() + WeightGroupTwo.Num();
}

void UNsNNTwoLayerFF::InitializeRegulators(const float InLearningRate, const float InDropoutRate, const float InL2RegulatorStrength)
{
    // Regulators -------------------------
    InitialLearningRate = InLearningRate;
    LearningRate = InLearningRate;
    InitialDropoutRate = InDropoutRate;
    DropoutRate = InDropoutRate;
    L2RegularizationStrength = InL2RegulatorStrength;
}

void UNsNNTwoLayerFF::UpdateRegularizationRates()
{
    if (const UNsNNSessionSubsystem* const NeuralTrainSubsystem = UNsNNSessionSubsystem::GetSubsystem())
    {
        if (NeuralTrainSubsystem->IsTraining())
        {
            const int32 CurrentTime = NeuralTrainSubsystem->GetCurrentTimePerIndividual();
            const int32 MaxTime = NeuralTrainSubsystem->GetMaxTimePerIndividual();
            // Exponential decay for learning rate
            const float DecayFactor = 0.95f;
            LearningRate = (InitialLearningRate * FMath::Pow(DecayFactor, static_cast<float>(CurrentTime) / static_cast<float>(MaxTime)));

            // Linearly decay dropout rate
            const float NewDropoutRate = InitialDropoutRate * (1.0f - static_cast<float>(CurrentTime) / static_cast<float>(MaxTime));
            DropoutRate = FMath::Clamp(NewDropoutRate, 0.05f, InitialDropoutRate); // Prevent extreme values
        }
    }
}

const TArray<float> UNsNNTwoLayerFF::ProcessInformation(const TArray<float>& InInputs, ANsNNRuntimeController* const InController)
{
    UpdateRegularizationRates();
    const TArray<float>& PredictedOutputs = ForwardPropagate(InInputs);
    return PredictedOutputs;
}

void UNsNNTwoLayerFF::SetNetworkWeights(const TArray<float>& InGenotype)
{
    const int32 RequiredGenotypeSize = (InputLayerSize + 1) * HiddenLayerSize + (HiddenLayerSize + 1) * OutputLayerSize;

    if (InGenotype.Num() != RequiredGenotypeSize)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid genotype size! Expected %d but got %d."), RequiredGenotypeSize, InGenotype.Num());
        return;
    }

    int32 GeneIndex = 0;

    for (int32 i = 0; i < HiddenLayerSize; ++i)
    {
        for (int32 j = 0; j < InputLayerSize; ++j)
        {
            const int32 Index = j + i * (InputLayerSize + 1);
            WeightGroupOne[Index] = InGenotype[GeneIndex++];
        }
        const int32 BiasIndex = i * (InputLayerSize + 1) + InputLayerSize;
        WeightGroupOne[BiasIndex] = InGenotype[GeneIndex++];
    }

    for (int32 i = 0; i < OutputLayerSize; ++i)
    {
        for (int32 j = 0; j < HiddenLayerSize; ++j)
        {
            const int32 Index = j + i * (HiddenLayerSize + 1);
            WeightGroupTwo[Index] = InGenotype[GeneIndex++];
        }
        const int32 BiasIndex = i * (HiddenLayerSize + 1) + HiddenLayerSize;
        WeightGroupTwo[BiasIndex] = InGenotype[GeneIndex++];
    }
}


TArray<float> UNsNNTwoLayerFF::ForwardPropagate(const TArray<float>& InInputs)
{
    if (InInputs.Num() != InputLayerSize)
    {
        // If you are here, take a look into your input feeding logic, the problem lies there
        UE_LOG(LogTemp, Error, TEXT("Input size mismatch! Expected %d but got %d."), InputLayerSize, InInputs.Num());
        return {};
    }

    // Calculate hidden layer outputs
    for (int32 i = 0; i < HiddenLayerSize; ++i)
    {
        float Sum = 0.f;
        for (int32 j = 0; j < InputLayerSize; ++j)
        {
            const int32 WeightIndex = j + i * (InputLayerSize + 1);
            Sum += InInputs[j] * WeightGroupOne[WeightIndex];
        }
        const int32 BiasIndex = i * (InputLayerSize + 1) + InputLayerSize;
        Sum -= WeightGroupOne[BiasIndex];
        HiddenLayerOutputs[i] = 1.f / (1.f + UKismetMathLibrary::Exp(-Sum)); // Sigmoid activation
    }

    // Calculate output layer outputs
    for (int32 i = 0; i < OutputLayerSize; ++i)
    {
        float Sum = 0.f;
        for (int32 j = 0; j < HiddenLayerSize; ++j)
        {
            const int32 WeightIndex = j + i * (HiddenLayerSize + 1);
            Sum += HiddenLayerOutputs[j] * WeightGroupTwo[WeightIndex];
        }
        const int32 BiasIndex = i * (HiddenLayerSize + 1) + HiddenLayerSize;
        Sum -= WeightGroupTwo[BiasIndex];
        OutputLayerOutputs[i] = 1.f / (1.f + UKismetMathLibrary::Exp(-Sum)); // Sigmoid activation
    }

    return OutputLayerOutputs;
}

TArray<FVector2D> UNsNNTwoLayerFF::GetNodePositions() const
{
    TArray<FVector2D> NodePositions;

    const float LayerSpacing = 100.f; // Spacing between layers
    const float NodeSpacing = 25.f;   // Spacing between nodes

    // Input layer
    for (int32 i = 0; i < InputLayerSize; ++i)
    {
        NodePositions.Emplace(FVector2D(0.f, i * NodeSpacing));
    }

    // Hidden layer
    for (int32 i = 0; i < HiddenLayerSize; ++i)
    {
        NodePositions.Emplace(FVector2D(LayerSpacing, i * NodeSpacing));
    }

    // Output layer
    for (int32 i = 0; i < OutputLayerSize; ++i)
    {
        NodePositions.Emplace(FVector2D(LayerSpacing * 2, i * NodeSpacing));
    }

    return NodePositions;
}

TArray<TTuple<int32, int32, float>> UNsNNTwoLayerFF::GetConnections() const
{
    TArray<TTuple<int32, int32, float>> Connections;

    // Input to Hidden
    for (int32 i = 0; i < HiddenLayerSize; ++i)
    {
        for (int32 j = 0; j < InputLayerSize; ++j)
        {
            const int32 InputNodeIndex = j; // Input node index (directly)
            const int32 HiddenNodeIndex = InputLayerSize + i; // Hidden node index adjusted by input layer size
            const int32 WeightIndex = j + i * (InputLayerSize + 1);
            Connections.Emplace(TTuple<int32, int32, float>(InputNodeIndex, HiddenNodeIndex, WeightGroupOne[WeightIndex]));
        }
    }

    // Hidden to Output
    for (int32 i = 0; i < OutputLayerSize; ++i)
    {
        for (int32 j = 0; j < HiddenLayerSize; ++j)
        {
            const int32 HiddenNodeIndex = j + InputLayerSize; // Hidden node index adjusted by input layer size
            const int32 OutputNodeIndex = HiddenLayerSize + InputLayerSize + i; // Output node index adjusted by hidden layer size and input size
            const int32 WeightIndex = j + i * (HiddenLayerSize + 1);
            Connections.Emplace(TTuple<int32, int32, float>(HiddenNodeIndex, OutputNodeIndex, WeightGroupTwo[WeightIndex]));
        }
    }

    return Connections;
}
