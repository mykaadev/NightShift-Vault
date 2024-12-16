// Copyright (C) 2024 mykaa. All rights reserved.

#include "Classes/NsUtilAIBaseConsideration.h"
#include "Kismet/KismetMathLibrary.h"

UNsUtilAIBaseConsideration::UNsUtilAIBaseConsideration()
    : Name(TEXT("Consideration Name"))
    , Score(0.f)
{
}
float UNsUtilAIBaseConsideration::GetScore(AActor* const InActor)
{
    if (InActor != nullptr)
    {
        return Score;
    }

    return 0.f;
}

void UNsUtilAIBaseConsideration::SetScore(const float InValue)
{
    Score = UKismetMathLibrary::NormalizeToRange(InValue, 0, 1);
}