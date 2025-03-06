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

    /** On Controller Spawn */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NN Neural Controller")
    void OnControllerSpawn(const class UClass* const InActorToSpawnAndControl, const FVector& InSpawnLocation, const FRotator& InSpawnRotation);
    virtual void OnControllerSpawn_Implementation(class UClass* const InActorToSpawnAndControl, const FVector& InSpawnLocation, const FRotator& InSpawnRotation);

    /** On Reset Requested */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NN Neural Controller")
    void OnResetRequested();
    virtual void OnResetRequested_Implementation();

    /** Compute Fitness */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NN Neural Controller")
    float ComputeFitness();
    virtual float ComputeFitness_Implementation();

    /** Update Fitness */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NN Neural Controller")
    void UpdateFitness(const float InDeltaTime);
    virtual void UpdateFitness_Implementation(const float InDeltaTime);

    /** Has Failed And Should Force Skip */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "NN Neural Controller")
    bool HasFailedAndShouldForceSkip();
    virtual bool HasFailedAndShouldForceSkip_Implementation();

#if WITH_EDITOR
    /** Focus Active Controlled Pawn */
    UFUNCTION(BlueprintCallable, Category = "NN Neural Controller")
    void FocusControlledPawn() const;
#endif // WITH_EDITOR

// Variables
public:

    /** Fitness */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Fitness;
};