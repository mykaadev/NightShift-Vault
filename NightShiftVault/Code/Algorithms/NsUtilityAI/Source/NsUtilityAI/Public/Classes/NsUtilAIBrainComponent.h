// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NsUtilAIBrainComponent.generated.h"

/**
 * Utility AI - Brain Component
 */
UCLASS(ClassGroup=(UtilityAI), Blueprintable, meta=(BlueprintSpawnableComponent, DisplayName = "Utility AI Brain"))
class NSUTILITYAI_API UNsUtilAIBrainComponent : public UActorComponent
{
    GENERATED_BODY()

// Functions
public:

    /** Constructor */
    UNsUtilAIBrainComponent();

    //~ Begin UActorComponent Interface
    virtual void BeginPlay() override;
    virtual void DestroyComponent(bool bPromoteChildren /* = false */) override;
    //~ End UActorComponent Interface

    /** Score Action */
    UFUNCTION()
    float ScoreAction(class UNsUtilAIBaseAction* InAction) const;

    /** Decide best Action */
    UFUNCTION()
    void ChooseAction(TArray<TSubclassOf<class UNsUtilAIBaseAction>>& InActions);

    /** Create Instances of all the owner actions */
    UFUNCTION()
    class UNsUtilAIBaseAction* GetBestAction() const;

    /** Chooses a new Action */
    UFUNCTION()
    void Think();

// Variables
public:

    /** Best Action */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    class UNsUtilAIBaseAction* BestAction;

    /** Timer handle for brain processes */
    UPROPERTY()
    FTimerHandle ThinkTimer;

    /** Possible Actions */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Setup", meta = (AllowPrivateAccess = "true"))
    TArray<TSubclassOf<class UNsUtilAIBaseAction>> PossibleActions;
};