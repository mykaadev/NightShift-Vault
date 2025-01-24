// Copyright (C) 2024 mykaa. All rights reserved.

#include "NsNeuralFramework.h"

#define LOCTEXT_NAMESPACE "FNsNeuralFrameworkModule"

void FNsNeuralFrameworkModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FNsNeuralFrameworkModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNsNeuralFrameworkModule, NsNeuralFramework)