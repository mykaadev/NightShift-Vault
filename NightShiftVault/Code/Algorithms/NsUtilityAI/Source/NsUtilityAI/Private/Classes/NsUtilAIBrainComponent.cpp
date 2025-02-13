// Copyright (C) 2024 mykaa. All rights reserved.

#include "Classes/NsUtilAIBrainComponent.h"
#include "Classes/NsUtilAIBaseAction.h"
#include "Classes/NsUtilAIBaseConsideration.h"

UNsUtilAIBrainComponent::UNsUtilAIBrainComponent()
    : BestAction(nullptr)
{
}

void UNsUtilAIBrainComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UNsUtilAIBrainComponent::DestroyComponent(bool bPromoteChildren /* = false */)
{
    Super::DestroyComponent(bPromoteChildren);
}

void UNsUtilAIBrainComponent::ChooseAction(TArray<TSubclassOf<UNsUtilAIBaseAction>>& InActions)
{
    if (!InActions.IsEmpty())
    {
        float Score = 0.f;
        int32 NextBestActionIndex = 0;

        for (int32 i = 0; i < InActions.Num(); ++i)
        {
            if (InActions[i] != nullptr && ScoreAction(InActions[i]->GetDefaultObject<UNsUtilAIBaseAction>()) > Score)
            {
                NextBestActionIndex = i;
                Score = InActions[i]->GetDefaultObject<UNsUtilAIBaseAction>()->GetScore();
            }
        }

        BestAction = InActions[NextBestActionIndex]->GetDefaultObject<UNsUtilAIBaseAction>();
    }
}

float UNsUtilAIBrainComponent::ScoreAction(UNsUtilAIBaseAction* InAction) const
{
    if (InAction != nullptr)
    {
        TArray<TSubclassOf<UNsUtilAIBaseConsideration>>& ActionConsiderations = InAction->GetConsideration();
        if (!ActionConsiderations.IsEmpty())
        {
            const float ModificationFactor = 1 - (1.f / ActionConsiderations.Num());
            float Score = 1.f;

            // Score all the considerations
            for (int32 i = 0; i < ActionConsiderations.Num(); ++i)
            {
                if (ActionConsiderations[i] != nullptr && GetOwner() != nullptr)
                {
                    if (UNsUtilAIBaseConsideration* const Consideration = ActionConsiderations[i]->GetDefaultObject<UNsUtilAIBaseConsideration>())
                    {
                        const float ConsiderationScore = Consideration->GetScore(GetOwner());
                        const float MakeUpValue = (1 - ConsiderationScore) * ModificationFactor;
                        const float FinalScore = ConsiderationScore + (MakeUpValue * ConsiderationScore);
                        Score *= FinalScore;
                    }
                }
            }

            // Set the action's score to the final result
            InAction->SetScore(Score);

            return InAction->GetScore();
        }
    }

    return 0.f;
}

UNsUtilAIBaseAction* UNsUtilAIBrainComponent::GetBestAction() const
{
    return BestAction;
}

void UNsUtilAIBrainComponent::Think()
{
    ChooseAction(PossibleActions);
}