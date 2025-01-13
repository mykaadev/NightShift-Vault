// Copyright (C) 2024 mykaa. All rights reserved.

#include "Networks/NsNNTwoLayerFeedForward.h"
#include "Kismet/KismetMathLibrary.h"

UNsNNTwoLayerFeedForward::UNsNNTwoLayerFeedForward()
    : InputLayerSize(0)
    , HiddenLayerSize(0)
    , OutputLayerSize(0)
{}

int32 UNsNNTwoLayerFeedForward::Initialize(const int32 InInputs, const int32 InHiddenLayer, const int32 InOutputs)
{
    InputLayerSize = InInputs;
    HiddenLayerSize = InHiddenLayer;
    OutputLayerSize = InOutputs;

    WeightGroupOne.SetNum((InputLayerSize + 1) * HiddenLayerSize);
    WeightGroupTwo.SetNum((HiddenLayerSize + 1) * OutputLayerSize);

    HiddenLayerOutputs.SetNum(HiddenLayerSize);
    OutputLayerOutputs.SetNum(OutputLayerSize);

    return WeightGroupOne.Num() + WeightGroupTwo.Num();
}

void UNsNNTwoLayerFeedForward::SetWeights(const TArray<float>& InGenotype)
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

const TArray<float> UNsNNTwoLayerFeedForward::ProcessInputs(const TArray<float>& InInputs)
{
    return ForwardPropagation(InInputs);
}

TArray<float> UNsNNTwoLayerFeedForward::ForwardPropagation(const TArray<float>& InInputs)
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
