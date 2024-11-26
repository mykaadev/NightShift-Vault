// Copyright © 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interaction Interface
 */
class NSINTERACTION_API IInteractable
{
    GENERATED_BODY()

// Functions
public:

    /** On enter the interaction radius */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void OnEnter(class AActor* InActor);
    virtual void OnEnter_Implementation(class AActor* const InActor) = 0;

    /** On exit the interaction radius */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void OnExit(class AActor* InActor);
    virtual void OnExit_Implementation(class AActor* const InActor) = 0;

    /** On interact with this object */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void OnInteract(class AActor* InActor);
    virtual void OnInteract_Implementation(class AActor* const InActor) = 0;
};
