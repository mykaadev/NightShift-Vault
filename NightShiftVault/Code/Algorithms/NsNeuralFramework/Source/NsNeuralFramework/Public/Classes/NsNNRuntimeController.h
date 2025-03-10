﻿// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "Libraries/NsNNFunctionLibrary.h"
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

    /** Destructor */
    ~ANsNNRuntimeController();

    //~ Begin AController Interface
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    //~ End AController Interface

    /** Initialize */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NN Neural Controller")
    void Initialize();
    virtual void Initialize_Implementation();

    /** Feed Inputs */
    UFUNCTION(BlueprintNativeEvent, Category = "NN Neural Controller")
    TArray<float> FeedInputs();
    virtual TArray<float> FeedInputs_Implementation();

    /** Feed Expected Outputs */
    UFUNCTION(BlueprintNativeEvent, Category = "NN Neural Controller")
    TArray<float> FeedExpectedOutputs();
    virtual TArray<float> FeedExpectedOutputs_Implementation();

    /** Handle Predicted Outputs */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NN Neural Controller")
    void HandlePredictedOutputs(const TArray<float>& InOutputs);
    virtual void HandlePredictedOutputs_Implementation(const TArray<float>& InOutputs);

    /** Callback for when the user  imports train data from a session file */
    UFUNCTION()
    void OnTrainDataImported(const TMap<FString, FString>& InParsedData, const class AController* const InController);

// Variables
public:

    /** Neural Network */
    UPROPERTY()
    TObjectPtr<class UNsNNArchitecture> NeuralNetwork;

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

    /** Stored Inputs */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<float> StoredInputs;

    /** Stored Outputs */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<float> StoredPredictedOutputs;
};