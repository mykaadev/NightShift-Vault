// Copyright (C) 2024 mykaa. All rights reserved.

#include "Classes/InteractionComponent.h"
#include "Classes/Interactable.h"

UInteractionComponent::UInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

UInteractionComponent::~UInteractionComponent()
{
}

void UInteractionComponent::AddToInteractionQueue(class AActor* InActor)
{
    if (InActor != nullptr && InActor->Implements<UInteractable>())
    {
        if (!InteractionQueue.Contains(InActor))
        {
            InteractionQueue.Emplace(InActor);

            if (AActor* const OwnerActor = GetOwner())
            {
                IInteractable::Execute_OnEnter(InActor, OwnerActor);
            }
        }
    }
}

void UInteractionComponent::RemoveFromInteractionQueue(class AActor* InActor)
{
    if (InActor != nullptr && InActor->Implements<UInteractable>())
    {
        if (InteractionQueue.Contains(InActor))
        {
            if (AActor* const OwnerActor = GetOwner())
            {
                IInteractable::Execute_OnExit(InActor, OwnerActor);
            }

            InteractionQueue.Remove(InActor);
        }
    }
}

AActor* UInteractionComponent::GetCurrentInteractableActor()
{
    return !InteractionQueue.IsEmpty() ? InteractionQueue.Last() : nullptr;
}

void UInteractionComponent::Interact()
{
    if (!InteractionQueue.IsEmpty())
    {
        AActor* const OwnerActor = GetOwner();
        AActor* const CurrentActor = GetCurrentInteractableActor();

        if (OwnerActor != nullptr && CurrentActor != nullptr && CurrentActor->Implements<UInteractable>())
        {
            IInteractable::Execute_OnInteract(CurrentActor, OwnerActor);
        }
    }
}