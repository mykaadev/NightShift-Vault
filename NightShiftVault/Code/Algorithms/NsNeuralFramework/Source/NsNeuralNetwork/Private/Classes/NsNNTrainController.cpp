// Copyright (C) 2024 mykaa. All rights reserved.

#include "NsNNTrainController.h"
#include "NsNNSessionSubsystem.h"

ANsNNTrainController::ANsNNTrainController()
    : NeuralSubsystem(nullptr)
    , Fitness(0)
{
    PrimaryActorTick.bCanEverTick = true;
}

void ANsNNTrainController::BeginPlay()
{
    if (UNsNNSessionSubsystem* const NeuralTrainSubsystem = UNsNNSessionSubsystem::GetSubsystem())
    {
        InputSize = NeuralTrainSubsystem->GetSessionData().Agent.NeuralInputs;
        HiddenLayerSize = NeuralTrainSubsystem->GetSessionData().Agent.NeuralHiddenLayerSize;
        OutputSize = NeuralTrainSubsystem->GetSessionData().Agent.NeuralOutputs;

        NeuralNetwork = NeuralTrainSubsystem->GetNeuralNetwork();
        NeuralSubsystem = NeuralTrainSubsystem;
    }

    Super::BeginPlay();
}

void ANsNNTrainController::Tick(float DeltaTime)
{
    if (NeuralNetwork != nullptr && NeuralSubsystem != nullptr)
    {
        if (HasFailedAndShouldForceSkip())
        {
            NeuralSubsystem->EndCurrentEvaluation();
        }
    }

    Super::Tick(DeltaTime);
}
void ANsNNTrainController::OnResetRequested_Implementation()
{
    // Should be extended by children
}

float ANsNNTrainController::ComputeFitness_Implementation()
{
    return Fitness;
}

bool ANsNNTrainController::HasFailedAndShouldForceSkip_Implementation()
{
    // Should be overriden by children
    return false;
}
