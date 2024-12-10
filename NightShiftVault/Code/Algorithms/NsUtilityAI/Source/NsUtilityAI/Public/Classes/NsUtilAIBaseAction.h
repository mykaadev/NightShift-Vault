// Copyright © 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NsUtilAIBaseAction.generated.h"

/**
 * Base Action for Utility AI
 */
UCLASS(Blueprintable)
class NSUTILITYAI_API UNsUtilAIBaseAction : public UObject
{
    GENERATED_BODY()

// Functions
protected:

    /** Constructor */
    UNsUtilAIBaseAction();

public:

    /** Get Score Normalized */
    UFUNCTION(BlueprintCallable)
    float GetScore() const;

    /** Set Score Normalized */
    UFUNCTION(BlueprintCallable)
    void SetScore(const float InValue);

    /** Executes Action */
    UFUNCTION(BlueprintCallable)
    virtual void ExecuteAction(AActor* const InActor);

    /** Gets Consideration Instances */
    UFUNCTION()
    TArray<TSubclassOf<class UNsUtilAIBaseConsideration>>& GetConsideration();

// Variables
public:

    /** Action Name */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FString Name;

    /** Action Score */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Score;

    /** Considerations */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    TArray<TSubclassOf<class UNsUtilAIBaseConsideration>> Considerations;

    /** Considerations */
    UPROPERTY(BlueprintReadOnly)
    class AActor* MyOwner;

};