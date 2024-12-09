// Copyright © 2024 mykaa. All rights reserved.

#include "NsNeuralNetwork.h"

#define LOCTEXT_NAMESPACE "FNsNeuralNetworkModule"

void FNsNeuralNetworkModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FNsNeuralNetworkModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNsNeuralNetworkModule, NsNeuralNetwork)