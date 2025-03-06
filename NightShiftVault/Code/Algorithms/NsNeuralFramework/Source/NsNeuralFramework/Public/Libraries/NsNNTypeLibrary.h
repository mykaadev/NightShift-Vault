// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NsNNTypeLibrary.generated.h"

/**
 * Neural Session Overrides
 */
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ENsTrainSessionOverride : uint8
{
    None                      = 0,      // Default train, no overrides
    InjectGenotype            = 1 << 0, // Start with a predefined genotype for individuals
    MutateGenotypeInjection   = 1 << 1, // Start with a predefined genotype for individuals
    InjectPopulation          = 1 << 2, // Start with a predefined population
    MutatePopulationInjection = 1 << 3  // Start with a predefined population
};
ENUM_CLASS_FLAGS(ENsTrainSessionOverride)

/**
 *  Struct used to hold Agent setups
 */
USTRUCT(BlueprintType)
struct FNsTrainAgentSetup
{
    GENERATED_BODY()

// Variables
public:

    /** Pawn */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftClassPtr<class APawn> Pawn;

    /** Controller */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftClassPtr<class ANsNNTrainController> Controller;

    /** Amount Neural Network Inputs */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NeuralInputs;

    /** Neural Network Hidden Layer Size */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NeuralHiddenLayerSize;

    /** Amount Neural Network Outputs */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NeuralOutputs;

    /** Training: Learning rate */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LearningRate;

    /** Training: Dropout rate */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DropoutRate;

    /** Training: L2 Regulator strength */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float L2RegulatorStrength;

// Functions
public:

    /** Constructor */
    FNsTrainAgentSetup()
        : NeuralInputs(1)
        , NeuralHiddenLayerSize(1)
        , NeuralOutputs(1)
    {}
};

/**
 * Struct to hold the gym setup for the session
 */
USTRUCT(BlueprintType)
struct FNsTrainGymSetup
{
    GENERATED_BODY()

    /** Level used for the train */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<class UWorld> Level;

    /** Spawn pool composed has location and rotation */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FVector, FRotator> SpawnPool;
};

/**
 *  Struct used to hold Population setups
 */
USTRUCT(BlueprintType)
struct FNsTrainPopulationSetup
{
    GENERATED_BODY()

// Variables
public:

    /** Population size for the train session */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PopulationSize;

    /** Max time in seconds per individual */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxTimePerIndividual;

    /** Mutation chance from 0 - 1 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MutationChance;

    /** Recombination chance from 0 - 1 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RecombinationChance;

// Functions
public:

    /** Constructor */
    FNsTrainPopulationSetup()
        : PopulationSize(20)
        , MaxTimePerIndividual(30)
        , MutationChance(0.05f)
        , RecombinationChance(0.8f)
    {}
};

/**
 *  Struct used to hold Session setups
 */
USTRUCT(BlueprintType)
struct FNsTrainSessionSetup
{
    GENERATED_BODY()

// Variables
public:

    /** Session name */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    /** Neural Network */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftClassPtr<class UNsNNArchitecture> NeuralNetwork;

    /** Population Setup */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FNsTrainPopulationSetup Population;

    /** Agent Setup */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FNsTrainAgentSetup Agent;

    /** Gym Setup */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FNsTrainGymSetup Gym;

// Functions
public:

    /** Constructor */
    FNsTrainSessionSetup()
    {}
};

/**
 * Struct to hold the session setups has row handles
 */
USTRUCT(BlueprintType)
struct FNsTrainSessionRowHandle : public FTableRowBase
{
    GENERATED_BODY()

// Variables
public:

    /** Session Setup */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FNsTrainSessionSetup SessionSetup;

// Functions
public:

    /** Constructor */
    FNsTrainSessionRowHandle()
    {}
};

/**
 * Neural Network Type Library
 */
UCLASS()
class NSNEURALFRAMEWORK_API UNsNNTypeLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
};