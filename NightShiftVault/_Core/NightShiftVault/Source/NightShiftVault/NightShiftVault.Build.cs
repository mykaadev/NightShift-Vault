﻿// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NightShiftVault : ModuleRules
{
    public NightShiftVault(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                // ... add public include paths required here ...
            }
        );

        PrivateIncludePaths.AddRange(
            new string[]
            {
                // ... add other private include paths required here ...
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                // ... add other public dependencies that you statically link with here ...

                //~ Begin Night Shift Vault
                // Algorithms
                "NsNeuralFramework",
                "NsUtilityAI",

                // Gameplay
                "NsAchievement",
                "NsFeedback",
                "NsInteraction",
                "NsSaveWizard",

                // Debug
                "NsConsole",
                "NsDialogue",
                "NsProjectMaid",

                // Visuals
                "NsTextFlow",
                "NsTween",
                //~ Begin Night Shift Vault
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                // ... add private dependencies that you statically link with here ...
            }
        );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                // ... add any modules that your module loads dynamically here ...
            }
        );
    }
}