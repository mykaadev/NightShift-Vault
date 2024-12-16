// Copyright (C) 2024 mykaa. All rights reserved.

#include "Classes/NsUtilAIBaseAction.h"
#include "Kismet/KismetMathLibrary.h"

UNsUtilAIBaseAction::UNsUtilAIBaseAction()
    : Name(TEXT("Action Name"))
    , Score(0.f)
    , MyOwner(nullptr)
{}

float UNsUtilAIBaseAction::GetScore() const
{
    return Score;
}

void UNsUtilAIBaseAction::ExecuteAction(AActor* const InActor)
{
}

void UNsUtilAIBaseAction::SetScore(const float InValue)
{
    Score = UKismetMathLibrary::NormalizeToRange(InValue, 0, 1);
}

TArray<TSubclassOf<class UNsUtilAIBaseConsideration>>& UNsUtilAIBaseAction::GetConsideration()
{
    return Considerations;
}