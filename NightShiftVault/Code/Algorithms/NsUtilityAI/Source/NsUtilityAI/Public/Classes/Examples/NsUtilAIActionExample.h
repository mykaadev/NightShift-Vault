// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "Classes/NsUtilAIBaseAction.h"
#include "NsUtilAIActionExample.generated.h"

/**
 * Example Action for Utility AI
 */
UCLASS()
class NSUTILITYAI_API UNsUtilAIActionExample : public UNsUtilAIBaseAction
{
    GENERATED_BODY()

// Functions
public:

    //~ Begin UMkUtilAIBaseAction Interface
    void ExecuteAction(AActor* const InActor)
    {
        Super::ExecuteAction(InActor);

        // Simple example on how to execute the action
        // if (ABaseEnemy* const EnemyBase = Cast<ABaseEnemy>(InActor))
        // {
        //     EnemyBase->ActionHeal();
        // }
    }
    //~ End UMkUtilAIBaseAction Interface
};