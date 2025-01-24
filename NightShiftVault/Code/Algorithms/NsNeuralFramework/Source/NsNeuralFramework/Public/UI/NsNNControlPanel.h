// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NsNNControlPanel.generated.h"

/**
 * Neural Network Control Panel
 */
UCLASS()
class NSNEURALFRAMEWORK_API UNsNNControlPanel : public UUserWidget
{
    GENERATED_BODY()

// Functions
public:

    //~ Begin UUserWidget Interface
    virtual void NativePreConstruct() override;
    //~ End UUserWidget Interface

    /** Starts the train session */
    UFUNCTION(BlueprintCallable)
    void RequestStartTrainSession() const;

    /** Stops the train session */
    UFUNCTION(BlueprintCallable)
    void RequestStopTrainSession() const;

// Variables
public:

    /** SpinBox to define Random Seed */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
    TObjectPtr<class USpinBox> SBRandomSeed;

    /** SpinBox to define Population Size */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
    TObjectPtr<class USpinBox> SBPopulationSize;

    /** SpinBox to define Individual Time */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
    TObjectPtr<class USpinBox> SBMaxIndividualTime;

    /** Slider to define Recombination Probability */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
    TObjectPtr<class USpinBox> SBRecombination;

    /** Slider to define Mutation Probability */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
    TObjectPtr<class USpinBox> SBMutation;

    /** Slider to define Learning Rate */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
    TObjectPtr<class USpinBox> SBLearningRate;

    /** Slider to define Dropout Rate */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
    TObjectPtr<class USpinBox> SBDropoutRate;

    /** Slider to define L2 Regulator */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
    TObjectPtr<class USpinBox> SBL2Regulator;

    /** SpinBox to define Random Seed */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
    TObjectPtr<class USpinBox> SBInputs;

    /** SpinBox to define Population Size */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
    TObjectPtr<class USpinBox> SBHiddenLayer;

    /** SpinBox to define Individual Time */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(BindWidget))
    TObjectPtr<class USpinBox> SBOutput;
};