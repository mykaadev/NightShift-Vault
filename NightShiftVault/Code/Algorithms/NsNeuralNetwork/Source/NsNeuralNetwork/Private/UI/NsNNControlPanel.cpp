// Copyright © 2024 mykaa. All rights reserved.

#include "UI/NsNNControlPanel.h"
#include "Classes/NsNNGameMode.h"
#include "Components/SpinBox.h"
#include "Kismet/GameplayStatics.h"

void UNsNNControlPanel::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (SBRandomSeed != nullptr && SBPopulationSize != nullptr && SBMaxIndividualTime != nullptr)
    {
        SBRandomSeed->SetDelta(1.f);

        SBPopulationSize->SetMinValue(4.f);
        SBPopulationSize->SetMinSliderValue(4.f);
        SBPopulationSize->SetDelta(1.f);

        SBMaxIndividualTime->SetMinValue(10.f);
        SBMaxIndividualTime->SetMinSliderValue(10.f);
        SBMaxIndividualTime->SetDelta(1.f);
    }
}

void UNsNNControlPanel::RequestStartTrainSession() const
{
    if (const UWorld* const World = GetWorld())
    {
        if (ANsNNGameMode* const NeuralNetworkGameMode = Cast<ANsNNGameMode>(UGameplayStatics::GetGameMode(World)))
        {
            if (SBRandomSeed != nullptr)
            {
                const int32 RandomSeed = FMath::TruncToInt32(SBRandomSeed->GetValue());
                NeuralNetworkGameMode->SetRandomSeed(RandomSeed);
            }

            if (SBPopulationSize != nullptr && SBMaxIndividualTime != nullptr)
            {
                const int32 PopulationSize = FMath::TruncToInt32(SBPopulationSize->GetValue());
                const int32 MaxIndividualTime = FMath::TruncToInt32(SBMaxIndividualTime->GetValue());
                NeuralNetworkGameMode->SetPopulationParameters(PopulationSize, MaxIndividualTime);
            }

            if (SBRecombination != nullptr && SBMutation != nullptr)
            {
                const float Mutation = SBMutation->GetValue();
                const float Recombination = SBRecombination->GetValue();
                NeuralNetworkGameMode->SetMutationAndRecombinationParameters(Mutation, Recombination);
            }

            if (SBInputs != nullptr && SBHiddenLayer != nullptr && SBOutput != nullptr)
            {
                const int32 InputSize = FMath::TruncToInt32(SBInputs->GetValue());
                const int32 HiddenLayerSize = FMath::TruncToInt32(SBHiddenLayer->GetValue());
                const int32 OutputSize = FMath::TruncToInt32(SBOutput->GetValue());
                NeuralNetworkGameMode->SetNeuralNetworkStructure(InputSize, HiddenLayerSize, OutputSize);
            }

            NeuralNetworkGameMode->OnStartRequestReceived();
        }
    }
}

void UNsNNControlPanel::RequestStopTrainSession() const
{
    if (const UWorld* const World = GetWorld())
    {
        if (ANsNNGameMode* const NeuralNetworkGameMode = Cast<ANsNNGameMode>(UGameplayStatics::GetGameMode(World)))
        {
            NeuralNetworkGameMode->StopTrainSession();
        }
    }
}