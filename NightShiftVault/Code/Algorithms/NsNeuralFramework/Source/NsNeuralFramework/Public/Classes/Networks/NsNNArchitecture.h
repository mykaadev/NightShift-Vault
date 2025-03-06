// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NsNNArchitecture.generated.h"

/**
 * Base Neural Network, any other network should derive from this
 */
UCLASS(BlueprintType, Abstract)
class NSNEURALFRAMEWORK_API UNsNNArchitecture : public UObject
{
    GENERATED_BODY()

// Functions
public:

    /** Constructor */
    UNsNNArchitecture()
        : L2RegularizationStrength(0.1f)
        , InitialDropoutRate(0.2f)
        , InitialLearningRate(0.1f)
        , DropoutRate(InitialDropoutRate)
        , LearningRate(InitialLearningRate)
    {}

    /** Setup Network */
    virtual int32 InitializeNetwork(const int32 InInputs, const int32 InHiddenLayer, const int32 InOutputs) PURE_VIRTUAL(InitializeNetwork, return INDEX_NONE; )

    /** Initialize Network */
    virtual void InitializeRegulators(const float InLearningRate, const float InDropoutRate, const float InL2RegulatorStrength) PURE_VIRTUAL(InitializeRegulators, return; )

    /** Set Network Weights */
    virtual void SetNetworkWeights(const TArray<float>& InGenotype) PURE_VIRTUAL(SetNetworkWeights, return; )

    /** Process Information */
    virtual const TArray<float> ProcessInformation(const TArray<float>& InInputs, class ANsNNRuntimeController* const InController) PURE_VIRTUAL(ProcessInformation, return {}; )

    /** On Fitness Compute */
    virtual float OnFitnessCompute(const float InFitness) PURE_VIRTUAL(OnFitnessCompute, return InFitness; )

    /** Update Regularization Rates */
    virtual void UpdateRegularizationRates() PURE_VIRTUAL(UpdateRegularizationRates, return; )

    /** Get node positions */
    virtual TArray<FVector2D> GetNodePositions() const PURE_VIRTUAL(GetNodePositions, return {}; )

    /** Get connections */
    virtual TArray<TTuple<int32, int32, float>> GetConnections() const PURE_VIRTUAL(GetConnections, return {}; )

    /** Get Node Values */
    virtual TArray<float> GetNodeValues() const PURE_VIRTUAL(GetNodeValues, return {}; )

// Variables
public:

    /** L2 Regularization Strength */
    UPROPERTY(EditAnywhere)
    float L2RegularizationStrength;

    /** Initial Dropout Rate */
    UPROPERTY(EditAnywhere)
    float InitialDropoutRate;

    /** Initial Learning Rate */
    UPROPERTY(EditAnywhere)
    float InitialLearningRate;

    /** Dropout Rate */
    UPROPERTY(EditAnywhere)
    float DropoutRate;

    /** Learning Rate */
    UPROPERTY(EditAnywhere)
    float LearningRate;
};