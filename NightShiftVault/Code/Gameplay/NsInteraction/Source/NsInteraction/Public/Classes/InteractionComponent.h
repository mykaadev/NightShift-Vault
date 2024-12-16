// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

/**
 * Interaction Component
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NSINTERACTION_API UInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

// Functions
public:

    /** Constructor */
    UInteractionComponent();

    /** Constructor */
    ~UInteractionComponent();

    /** Adds an item to the interaction queue */
    UFUNCTION(BlueprintCallable)
    void AddToInteractionQueue(class AActor* InActor);

    /** Removes an item from the interaction queue */
    UFUNCTION(BlueprintCallable)
    void RemoveFromInteractionQueue(class AActor* InActor);

    /** Gets the current interactable actor */
    UFUNCTION(BlueprintCallable)
    class AActor* GetCurrentInteractableActor();

    /** Interact with the current object */
    UFUNCTION(BlueprintCallable)
    void Interact();

// Variables
public:

    /** Holds all the Current Interactable Actors */
    UPROPERTY(BlueprintReadWrite)
    TArray<class AActor*> InteractionQueue;
};
