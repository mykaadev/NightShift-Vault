// Copyright (C) 2024 mykaa. All rights reserved.

#include "Networks/NsNNRegularizedMLP.h"
#include "Kismet/KismetMathLibrary.h"
#include "NsNNRuntimeController.h"
#include "NsNNSessionSubsystem.h"

UNsNNRegularizedMLP::UNsNNRegularizedMLP()
    : InputLayerSize(INDEX_NONE)
    , HiddenLayerSize(INDEX_NONE)
    , OutputLayerSize(INDEX_NONE)
{
}

int32 UNsNNRegularizedMLP::InitializeNetwork(const int32 InInputs, const int32 InHiddenLayer, const int32 InOutputs)
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

void UNsNNRegularizedMLP::InitializeRegulators(const float InLearningRate, const float InDropoutRate,const float InL2RegulatorStrength)
{
    // Regulators -------------------------
    InitialLearningRate = InLearningRate;
    LearningRate = InLearningRate;
    InitialDropoutRate = InDropoutRate;
    DropoutRate = InDropoutRate;
    L2RegularizationStrength = InL2RegulatorStrength;
}

void UNsNNRegularizedMLP::SetNetworkWeights(const TArray<float>& InGenotype)
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

const TArray<float> UNsNNRegularizedMLP::ProcessInformation(const TArray<float>& InInputs, ANsNNRuntimeController* const InController)
{
    UpdateRegularizationRates();
    const TArray<float>& PredictedOutputs = ForwardPropagate(InInputs);

    // If training, perform backpropagation to adjust weights
    if (const UNsNNSessionSubsystem* const NeuralTrainSubsystem = UNsNNSessionSubsystem::GetSubsystem())
    {
        if (NeuralTrainSubsystem->IsTraining() && InController != nullptr)
        {
            // Compute the error between the expected outputs and target outputs
            const TArray<float>& ExpectedOutputs = InController->FeedExpectedOutputs();
            const TArray<float> Errors = ComputeError(PredictedOutputs, ExpectedOutputs);
            BackPropagate(Errors); // Perform backpropagation
        }
    }
    return PredictedOutputs;
}

TArray<float> UNsNNRegularizedMLP::ForwardPropagate(const TArray<float>& InInputs)
{
    if (InInputs.Num() != InputLayerSize)
    {
        UE_LOG(LogTemp, Error, TEXT("Input size mismatch! Expected %d but got %d."), InputLayerSize, InInputs.Num());
        return {};
    }

    // Store the input for later use in backpropagation
    StoredInputs = InInputs;

    // Proceed with the forward propagation as usual
    if (const UNsNNSessionSubsystem* const NeuralTrainSubsystem = UNsNNSessionSubsystem::GetSubsystem())
    {
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

            // Apply Dropout during training
            {
                if (NeuralTrainSubsystem->IsTraining() && UKismetMathLibrary::RandomFloatInRange(0.f, 1.f) < DropoutRate)
                {
                    HiddenLayerOutputs[i] = 0.f;
                }
            }
        }
    }

    // Calculate output layer outputs with L2 regularization
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

void UNsNNRegularizedMLP::BackPropagate(const TArray<float>& InErrors)
{
    // Initialize gradients for both weight groups
    TArray<float> GradientForWeightGroupOne;
    TArray<float> GradientForWeightGroupTwo;
    GradientForWeightGroupOne.SetNum(WeightGroupOne.Num());
    GradientForWeightGroupTwo.SetNum(WeightGroupTwo.Num());

    // Calculate gradients for the output layer
    for (int32 i = 0; i < OutputLayerSize; ++i)
    {
        // Compute error gradient for the output layer using sigmoid derivative
        const float OutputError = InErrors[i] * GetSigmoidDerivative(OutputLayerOutputs[i]);

        for (int32 j = 0; j < HiddenLayerSize; ++j)
        {
            const int32 WeightIndex = j + i * (HiddenLayerSize + 1);
            GradientForWeightGroupTwo[WeightIndex] = OutputError * HiddenLayerOutputs[j];
        }

        // Update bias for the output layer
        const int32 BiasIndex = i * (HiddenLayerSize + 1) + HiddenLayerSize;
        GradientForWeightGroupTwo[BiasIndex] = OutputError;
    }

    // Calculate gradients for the hidden layer
    for (int32 i = 0; i < HiddenLayerSize; ++i)
    {
        float HiddenError = 0.f;

        for (int32 j = 0; j < OutputLayerSize; ++j)
        {
            const int32 WeightIndex = i + j * (HiddenLayerSize + 1);
            HiddenError += InErrors[j] * WeightGroupTwo[WeightIndex] * GetSigmoidDerivative(HiddenLayerOutputs[i]);
        }

        for (int32 j = 0; j < InputLayerSize; ++j)
        {
            const int32 WeightIndex = j + i * (InputLayerSize + 1);
            GradientForWeightGroupOne[WeightIndex] = HiddenError * StoredInputs[j];
        }

        // Update bias for the hidden layer
        const int32 BiasIndex = i * (InputLayerSize + 1) + InputLayerSize;
        GradientForWeightGroupOne[BiasIndex] = HiddenError;
    }

    // Apply L2 regularization to gradients
    for (int32 i = 0; i < WeightGroupOne.Num(); ++i)
    {
        GradientForWeightGroupOne[i] += L2RegularizationStrength * WeightGroupOne[i];
    }

    for (int32 i = 0; i < WeightGroupTwo.Num(); ++i)
    {
        GradientForWeightGroupTwo[i] += L2RegularizationStrength * WeightGroupTwo[i];
    }

    // Update weights using gradients and learning rate
    for (int32 i = 0; i < WeightGroupOne.Num(); ++i)
    {
        WeightGroupOne[i] -= LearningRate * GradientForWeightGroupOne[i];
    }

    for (int32 i = 0; i < WeightGroupTwo.Num(); ++i)
    {
        WeightGroupTwo[i] -= LearningRate * GradientForWeightGroupTwo[i];
    }
}

void UNsNNRegularizedMLP::UpdateRegularizationRates()
{
    if (const UNsNNSessionSubsystem* const NeuralTrainSubsystem = UNsNNSessionSubsystem::GetSubsystem())
    {
        if (NeuralTrainSubsystem->IsTraining())
        {
            const int32 CurrentTime = NeuralTrainSubsystem->GetCurrentTimePerIndividual();
            const int32 MaxTime = NeuralTrainSubsystem->GetMaxTimePerIndividual();
            const float DecayFactor = 0.95f;
            LearningRate = InitialLearningRate / (1.0f + DecayFactor * (static_cast<float>(CurrentTime) / static_cast<float>(MaxTime)));

            const float NewDropoutRate = InitialDropoutRate * (1.0f - static_cast<float>(CurrentTime) / static_cast<float>(MaxTime));
            DropoutRate = FMath::Clamp(NewDropoutRate, 0.05f, InitialDropoutRate); // Prevent extreme values
        }
    }
}

float UNsNNRegularizedMLP::OnFitnessCompute(const float InFitness)
{
    // Apply L2 Regularization to the fitness score (penalize larger weights)
    const float L2Penalty = GetL2RegularizationPenalty();
    return InFitness - L2Penalty;
}

float UNsNNRegularizedMLP::GetL2RegularizationPenalty() const
{
    float L2Penalty = 0.f;

    // Sum of squares of all weights (L2 regularization)
    for (const float Weight : WeightGroupOne)
    {
        L2Penalty += FMath::Pow(Weight, 2);
    }
    for (const float Weight : WeightGroupTwo)
    {
        L2Penalty += FMath::Pow(Weight, 2);
    }

    // Regularize with L2Strength
    return L2RegularizationStrength * L2Penalty;
}

TArray<float> UNsNNRegularizedMLP::ComputeError(const TArray<float>& InPredictedOutputs, const TArray<float>& InExpectedOutputs) const
{
    TArray<float> Errors;

    // Ensure sizes match
    if (InPredictedOutputs.Num() != InExpectedOutputs.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("Output and target sizes do not match!"));
        return Errors;
    }

    Errors.SetNum(InPredictedOutputs.Num());

    // Calculate raw differences (Expected - Predicted) for gradients
    for (int32 i = 0; i < InPredictedOutputs.Num(); ++i)
    {
        Errors[i] = InExpectedOutputs[i] - InPredictedOutputs[i];
    }

    return Errors;
}

TArray<FVector2D> UNsNNRegularizedMLP::GetNodePositions() const
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

TArray<TTuple<int32, int32, float>> UNsNNRegularizedMLP::GetConnections() const
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

float UNsNNRegularizedMLP::GetSigmoidDerivative(const float InValue) const
{
    return InValue * (1.0f - InValue); // Sigmoid derivative: f'(x) = f(x) * (1 - f(x))
}

TArray<float> UNsNNRegularizedMLP::GetNodeValues() const
{
    TArray<float> NodeValues;

    // Append input values (assumed last input used in ForwardPropagate)
    NodeValues.Append(InputLayerValues);

    // Append hidden layer activations
    NodeValues.Append(HiddenLayerOutputs);

    // Append output layer activations
    NodeValues.Append(OutputLayerOutputs);

    return NodeValues;
}
