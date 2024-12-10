// Copyright © 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NsUtilAIBaseConsideration.generated.h"

/**
 * Base Consideration for Utility AI
 */
UCLASS(Blueprintable)
class NSUTILITYAI_API UNsUtilAIBaseConsideration : public UObject
{
    GENERATED_BODY()

// Functions
protected:

    /** Constructor */
    UNsUtilAIBaseConsideration();

public:

    /** Get Score Normalized */
    UFUNCTION(BlueprintCallable)
    virtual float GetScore(AActor* const InActor);

    /** Set Score Normalized */
    UFUNCTION(BlueprintCallable)
    void SetScore(const float InValue);

// Variables
public:

    /** Consideration Name */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FString Name;

    /** Consideration Score */
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    float Score;

    /** Consideration Curve */
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FRuntimeFloatCurve ConsiderationCurve;
};