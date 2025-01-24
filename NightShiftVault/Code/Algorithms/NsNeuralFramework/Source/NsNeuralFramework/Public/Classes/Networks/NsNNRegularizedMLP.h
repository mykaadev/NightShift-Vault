// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NsNNArchitecture.h"
#include "NsNNRegularizedMLP.generated.h"

/**
 * Regularized Multi Layer Perceptron
 */
UCLASS()
class NSNEURALFRAMEWORK_API UNsNNRegularizedMLP : public UNsNNArchitecture
{
    GENERATED_BODY()

// Functions
public:

    /** Constructor */
    UNsNNRegularizedMLP();

    //~ Begin UNsNNArchitecture Interface
    virtual int32 InitializeNetwork(const int32 InInputs, const int32 InHiddenLayer, const int32 InOutputs) override;
    virtual void InitializeRegulators(const float InLearningRate, const float InDropoutRate, const float InL2RegulatorStrength) override;
    virtual void SetNetworkWeights(const TArray<float>& InGenotype) override;
    virtual const TArray<float> ProcessInformation(const TArray<float>& InInputs, class ANsNNRuntimeController* const InController) override;
    virtual void UpdateRegularizationRates() override;
    virtual float OnFitnessCompute(const float InFitness) override;
    virtual TArray<FVector2D> GetNodePositions() const override;
    virtual TArray<TTuple<int32, int32, float>> GetConnections() const override;
    //~ End UNsNNArchitecture Interface

    /** Forward Propagate */
    TArray<float> ForwardPropagate(const TArray<float>& InInputs);

    /** Back Propagate */
    void BackPropagate(const TArray<float>& InErrors);

    /** Compute Errors */
    TArray<float> ComputeError(const TArray<float>& InPredictedOutputs, const TArray<float>& InExpectedOutputs) const;

    /** Get L2 Regularization penalty */
    float GetL2RegularizationPenalty() const;

    /** Sigmoid Derivative */
    float GetSigmoidDerivative(const float InValue) const;

// Variables
public:

    /** Amount Neural Network Inputs */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    int32 InputLayerSize;

    /** Neural Network Hidden Layer Size */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    int32 HiddenLayerSize;

    /** Amount Neural Network Outputs */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    int32 OutputLayerSize;

    /** First Weight Group */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<float> WeightGroupOne;

    /** Second Weight Group */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<float> WeightGroupTwo;

    /** Hidden Layer Outputs */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<float> HiddenLayerOutputs;

    /** Output Layer Outputs */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<float> OutputLayerOutputs;

    /** Stored inputs */
    TArray<float> StoredInputs;
};