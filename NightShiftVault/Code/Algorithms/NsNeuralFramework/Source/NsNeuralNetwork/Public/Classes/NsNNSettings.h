// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NsNNSettings.generated.h"

/**
 * Neural Network Settings
 */
UCLASS(Config=Plugins, DefaultConfig)
class NSNEURALFRAMEWORK_API UNsNNSettings : public UDeveloperSettings
{
    GENERATED_BODY()

// Variables
public:

    /** Neural Network control panel */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSoftClassPtr<class UNsNNControlPanel> ControlPanel;

    /** Sessions Data Table */
    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Data")
    TSoftObjectPtr<class UDataTable> SessionDataTable;

    /** Data Export Path */
    UPROPERTY(Config, EditAnywhere, Category = "Export")
    FString DataExportPath;
};
