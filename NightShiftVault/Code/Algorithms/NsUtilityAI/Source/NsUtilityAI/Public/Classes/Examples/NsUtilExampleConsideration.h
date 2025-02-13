// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Classes/NsUtilAIBaseConsideration.h"
#include "NsUtilExampleConsideration.generated.h"

/**
 * Example action consideration
 */
UCLASS()
class NSUTILITYAI_API UNsUtilExampleConsideration : public UNsUtilAIBaseConsideration
{
    GENERATED_BODY()

    //~ Begin UNsUtilAIBaseConsideration Interface
    virtual float GetScore(AActor* const InActor) override
    {
        // Example consideration usage
        // if (InActor != nullptr)
        // {
        //     float PlayerCurrentHealth = UCombatFunctionLibrary::GetPlayerStats().CurrentHP;
        //     float PlayerMaxHealth = UCombatFunctionLibrary::GetPlayerStats().MaxHP;
        //
        //     float NormalizedConsideration = UKismetMathLibrary::NormalizeToRange(PlayerCurrentHealth / PlayerMaxHealth, 0, 1);
        //     Score = ConsiderationCurve.GetRichCurve()->Eval(NormalizedConsideration);
        //
        //     return Score;
        // }

        return 0.f;
    }
    //~ Begin UNsUtilAIBaseConsideration Interface
};