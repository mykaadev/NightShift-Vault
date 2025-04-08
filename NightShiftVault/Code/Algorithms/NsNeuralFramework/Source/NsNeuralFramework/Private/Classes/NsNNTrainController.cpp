// Copyright (C) 2024 mykaa. All rights reserved.

#include "NsNNTrainController.h"
#include "Networks/NsNNArchitecture.h"
#include "NsNNSessionSubsystem.h"
#include "NsNNUserSettings.h"
#if WITH_EDITOR
#include "Kismet2/DebuggerCommands.h"
#include "LevelEditor.h"
#include "Selection.h"
#include "SLevelViewport.h"
#endif // WITH_EDITOR

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
            UpdateFitness(DeltaTime);
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

void ANsNNTrainController::OnControllerSpawn_Implementation(class UClass* InActorToSpawnAndControl, const FVector& InSpawnLocation, const FRotator& InSpawnRotation)
{
    if (UWorld* const World = GetWorld())
    {
        FActorSpawnParameters SpawnInfo;
        SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        if (APawn* const SpawnedPawn = World->SpawnActor<APawn>(InActorToSpawnAndControl, InSpawnLocation, InSpawnRotation, SpawnInfo))
        {
            Possess(SpawnedPawn);
#if WITH_EDITOR
            if (const UNsNNUserSettings* const UserSettings = GetDefault<UNsNNUserSettings>())
            {
                if (UserSettings->bAutoFocus)
                {
                    FocusControlledPawn();
                }
            }
#endif // WITH_EDITOR
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

void ANsNNTrainController::UpdateFitness_Implementation(const float InDeltaTime)
{
    // Should be overriden by children
}

bool ANsNNTrainController::HasFailedAndShouldForceSkip_Implementation()
{
    // Should be overriden by children
    return false;
}

#if WITH_EDITOR
void ANsNNTrainController::FocusControlledPawn() const
{
    if (GEditor != nullptr && GetPawn() != nullptr)
    {
        GEditor->SelectActor(GetPawn(), true, true);
        const USelection* const Selection = GEditor->GetSelectedActors();
        if (Selection != nullptr &&  Selection->Num() == 1)
        {
            if (FPlayWorldCommandCallbacks::IsInPIE())
            {
                GEditor->RequestToggleBetweenPIEandSIE();
            }

            FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
            if (const TSharedPtr<SLevelViewport> ActiveLevelViewport = LevelEditorModule.GetFirstActiveLevelViewport())
            {
                FLevelEditorViewportClient& LevelViewportClient = ActiveLevelViewport->GetLevelViewportClient();
                LevelViewportClient.SetActorLock(GetPawn());
                LevelViewportClient.MoveCameraToLockedActor();
            }
        }
    }
}
#endif //WITH_EDITOR
