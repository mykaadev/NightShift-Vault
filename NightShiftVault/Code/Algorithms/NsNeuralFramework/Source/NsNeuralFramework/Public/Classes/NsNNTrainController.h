// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NsNNRuntimeController.h"
#include "NsNNTrainController.generated.h"

/**
 * Neural Network Train Controller to be extended for each train
 */
UCLASS(Blueprintable)
class NSNEURALFRAMEWORK_API ANsNNTrainController : public ANsNNRuntimeController
{
    GENERATED_BODY()

// Functions
public:

    /** Constructor */
    ANsNNTrainController();

    //~ Begin AController Interface
    virtual void Tick(float DeltaTime) override;
    //~ End AController Interface

    //~ Begin ANsRuntimeController Interface
    virtual void Initialize_Implementation() override;
    //~ End ANsRuntimeController Interface

    /** On Reset Requested */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NN Neural Controller")
    void OnResetRequested();
    virtual void OnResetRequested_Implementation();

    /** Compute Fitness */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NN Neural Controller")
    float ComputeFitness();
    virtual float ComputeFitness_Implementation();

    /** Has Failed And Should Force Skip */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NN Neural Controller")
    bool HasFailedAndShouldForceSkip();
    virtual bool HasFailedAndShouldForceSkip_Implementation();

// Variables
public:

    /** Fitness */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Fitness;
};