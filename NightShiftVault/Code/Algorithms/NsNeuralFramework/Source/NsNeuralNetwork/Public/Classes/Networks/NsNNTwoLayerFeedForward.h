// Copyright © 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NsNNBaseNetwork.h"
#include "NsNNTwoLayerFeedForward.generated.h"

/**
 * Two layer feed forward neural network
 */
UCLASS()
class NSNEURALFRAMEWORK_API UNsNNTwoLayerFeedForward : public UNsNNBaseNetwork
{
    GENERATED_BODY()

// Functions
public:

    /** Constructor */
    UNsNNTwoLayerFeedForward();

    //~ Begin UNsNNBaseNetwork Interface
    virtual int32 Initialize(const int32 InInputs, const int32 InHiddenLayer, const int32 InOutputs) override;
    virtual void SetWeights(const TArray<float>& InGenotype) override;
    virtual const TArray<float> ProcessInputs(const TArray<float>& InInputs) override;
    //~ End UNsNNBaseNetwork Interface

    /** Forward Propagation */
    UFUNCTION()
    TArray<float> ForwardPropagation(const TArray<float>& InInputs);

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
    TArray<float> OutputLayerOutput;
};