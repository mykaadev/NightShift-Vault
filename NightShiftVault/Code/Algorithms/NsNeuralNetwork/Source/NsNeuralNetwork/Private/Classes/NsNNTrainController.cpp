// Copyright © 2024 mykaa. All rights reserved.

#include "Classes/NsNNTrainController.h"
#include "Classes/NsNNGameMode.h"
#include "Classes/Networks/NsNNBaseNetwork.h"
#include "Kismet/GameplayStatics.h"

ANsNNTrainController::ANsNNTrainController()
    : NNGameMode(nullptr)
    , NeuralNetwork(nullptr)
    , Fitness(0)
{
    PrimaryActorTick.bCanEverTick = true;
}

void ANsNNTrainController::BeginPlay()
{
    Super::BeginPlay();

    if (const UWorld* const World = GetWorld())
    {
        NNGameMode = Cast<ANsNNGameMode>(UGameplayStatics::GetGameMode(World));
        if (NNGameMode != nullptr)
        {
            NeuralNetwork = NNGameMode->GetNeuralNetwork();
        }
    }
}

void ANsNNTrainController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    InputValues.Empty();

    if (NNGameMode != nullptr && NeuralNetwork != nullptr)
    {
        if (HasFailedAndShouldForceSkip())
        {
            NNGameMode->EndCurrentEvaluation();
        }
        else
        {
            FeedInputs();
            ensureMsgf(InputValues.Num() == NNGameMode->GetInputSize(), TEXT("NsNNTrainController::Tick : Amount of inputs fed do not match with the Neural Network Session configuration"));
            const TArray<float>& Outputs = NeuralNetwork->ProcessInputs(InputValues);
            HandleOutputs(Outputs);
        }
    }
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

void ANsNNTrainController::FeedInputs_Implementation()
{
    // Should be overriden by children
}

void ANsNNTrainController::HandleOutputs_Implementation(const TArray<float>& InOutputs)
{
    // Should be overriden by children
}