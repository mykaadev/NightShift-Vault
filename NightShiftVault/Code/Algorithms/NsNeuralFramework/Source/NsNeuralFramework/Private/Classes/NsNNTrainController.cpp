// Copyright (C) 2024 mykaa. All rights reserved.

#include "NsNNTrainController.h"

#include "Networks/NsNNArchitecture.h"
#include "NsNNSessionSubsystem.h"

ANsNNTrainController::ANsNNTrainController()
  : Fitness(0)
{
    PrimaryActorTick.bCanEverTick = true;
}

void ANsNNTrainController::Tick(float DeltaTime)
{
    if (UNsNNSessionSubsystem* const NeuralTrainSubsystem = UNsNNSessionSubsystem::GetSubsystem())
    {
        if (NeuralNetwork != nullptr && NeuralTrainSubsystem->IsTraining())
        {
            if (HasFailedAndShouldForceSkip())
            {
                NeuralTrainSubsystem->EndCurrentEvaluation();
            }
        }
    }

    // Perform the actual handling of inputs and outputs
    Super::Tick(DeltaTime);
}

void ANsNNTrainController::Initialize_Implementation()
{
    if (const UNsNNSessionSubsystem* const NeuralTrainSubsystem = UNsNNSessionSubsystem::GetSubsystem())
    {
        if (NeuralTrainSubsystem->IsTraining())
        {
            const FNsTrainAgentSetup SessionAgentData = NeuralTrainSubsystem->GetSessionData().Agent;

            // Agent ----------------------------------
            InputSize = SessionAgentData.NeuralInputs;
            HiddenLayerSize = SessionAgentData.NeuralHiddenLayerSize;
            OutputSize = SessionAgentData.NeuralOutputs;

            // Regulators -----------------------------
            const float LearningRate = SessionAgentData.LearningRate;
            const float DropoutRate = SessionAgentData.DropoutRate;
            const float L2Regulator = SessionAgentData.L2RegulatorStrength;

            // Network --------------------------------
            NeuralNetwork = NeuralTrainSubsystem->GetNeuralNetwork();
            NeuralNetwork->InitializeNetwork(InputSize, HiddenLayerSize, OutputSize);
            NeuralNetwork->InitializeRegulators(LearningRate, DropoutRate, L2Regulator);
            NeuralNetwork->SetNetworkWeights(Genotype);
        }
    }
}

void ANsNNTrainController::OnResetRequested_Implementation()
{
    // Should be extended by children
}

float ANsNNTrainController::ComputeFitness_Implementation()
{
    if (NeuralNetwork != nullptr)
    {
        return NeuralNetwork->OnFitnessCompute(Fitness);
    }
    return Fitness;
}

bool ANsNNTrainController::HasFailedAndShouldForceSkip_Implementation()
{
    // Should be overriden by children
    return false;
}
