// Copyright © 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NsNNBaseNetwork.generated.h"

/**
 * Base Neural Network, any other network should derive from this
 */
UCLASS(BlueprintType)
class NSNEURALNETWORK_API UNsNNBaseNetwork : public UObject
{
    GENERATED_BODY()

// Functions
public:

    /** Constructor */
    UNsNNBaseNetwork() {}

    /** Initialize */
    virtual int32 Initialize(const int32 InInputs, const int32 InHiddenLayer, const int32 InOutputs) { return 0; }

    /** Set Weights */
    virtual void SetWeights(const TArray<float>& InGenotype) {}

    /** Feed Inputs and return the processed outputs */
    virtual const TArray<float> ProcessInputs(const TArray<float>& InInputs) { return {}; }
};
