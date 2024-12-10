// Copyright © 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "NsNNTrainController.generated.h"

/**
 * Neural Network Train Controller to be extended for each train
 */
UCLASS(Blueprintable)
class NSNEURALFRAMEWORK_API ANsNNTrainController : public AController
{
    GENERATED_BODY()

// Functions
public:

    /** Constructor */
    ANsNNTrainController();

    //~ Begin AController Interface
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    //~ End AController Interface

    /** On Reset Requested */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
    void OnResetRequested();
    virtual void OnResetRequested_Implementation();

    /** Compute Fitness */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
    float ComputeFitness();
    virtual float ComputeFitness_Implementation();

    /** Has Failed And Should Force Skip */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
    bool HasFailedAndShouldForceSkip();
    virtual bool HasFailedAndShouldForceSkip_Implementation();

    /** Feed Inputs */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
    void FeedInputs();
    virtual void FeedInputs_Implementation();

    /** Handle Outputs */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Controller")
    void HandleOutputs(const TArray<float>& InOutputs);
    virtual void HandleOutputs_Implementation(const TArray<float>& InOutputs);

// Variables
public:

    /** Inputs */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> InputValues;

    /** Neural Network Mode */
    UPROPERTY()
    TObjectPtr<class UNsNNBaseNetwork> NeuralNetwork;

    /** Fitness */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Fitness;
};