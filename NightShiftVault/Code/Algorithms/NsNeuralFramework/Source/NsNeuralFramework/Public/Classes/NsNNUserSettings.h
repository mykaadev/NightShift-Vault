// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NsNNUserSettings.generated.h"

/**
 * Night shift neural framework user settings
 */
UCLASS(config = EditorPerProjectUserSettings)
class NSNEURALFRAMEWORK_API UNsNNUserSettings : public UDeveloperSettings
{
    GENERATED_BODY()

    /** Constructor */
    UNsNNUserSettings()
        : bAutoSave(false)
        , bAutoFocus(false)
    {}

// Functions
public:

    /** Get mutable settings */
    UFUNCTION(BlueprintPure, Category = "Ns Neural Framework")
    static class UNsNNUserSettings* GetMutableSettings()
    {
        static UNsNNUserSettings* const StaticInstance = UNsNNUserSettings::StaticClass() != nullptr ? CastChecked<UNsNNUserSettings>(UNsNNUserSettings::StaticClass()->GetDefaultObject()) : nullptr;
        return StaticInstance;
    }

// Variables
public:

    /** Whether to auto save genotypes */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Ns Neural Framework")
    bool bAutoSave;

    /** Whether to auto focus agents */
    UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Ns Neural Framework")
    bool bAutoFocus;
};