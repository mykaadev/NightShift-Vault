// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "NsNNRuntimeController.generated.h"

/**
 * Neural Network Runtime Controller to be used with the trained genotype
 */
UCLASS(Blueprintable)
class NSNEURALFRAMEWORK_API ANsNNRuntimeController : public AController
{
    GENERATED_BODY()

// Functions
public:

    /** Constructor */
    ANsNNRuntimeController();

    //~ Begin AController Interface
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    //~ End AController Interface

    /** Feed Inputs */
    UFUNCTION(BlueprintNativeEvent, Category = "NN Neural Controller")
    TArray<float> FeedInputs();
    virtual TArray<float> FeedInputs_Implementation();

    /** Handle Outputs */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NN Neural Controller")
    void HandleOutputs(const TArray<float>& InOutputs);
    virtual void HandleOutputs_Implementation(const TArray<float>& InOutputs);

// Variables
public:

    /** Neural Network to use */
    UPROPERTY(EditAnywhere)
    TSubclassOf<class UNsNNBaseNetwork> NeuralNetworkClass;

    /** Neural Network */
    UPROPERTY()
    TObjectPtr<class UNsNNBaseNetwork> NeuralNetwork;

    /** Inputs size */
    UPROPERTY(EditAnywhere)
    int32 InputSize;

    /** Hidden Layer size */
    UPROPERTY(EditAnywhere)
    int32 HiddenLayerSize;

    /** Outputs size */
    UPROPERTY(EditAnywhere)
    int32 OutputSize;

    /** Genotype */
    UPROPERTY(EditAnywhere)
    TArray<float> Genotype;
};