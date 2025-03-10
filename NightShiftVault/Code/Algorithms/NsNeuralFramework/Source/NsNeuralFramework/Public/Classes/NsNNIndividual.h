﻿// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NsNNIndividual.generated.h"

/**
 * Neural Network Individual
 */
UCLASS()
class NSNEURALFRAMEWORK_API UNsNNIndividual : public UObject
{
    GENERATED_BODY()

// Functions
public:

    /** Constructor */
    UNsNNIndividual();

    /** Construct */
    UFUNCTION()
    void Construct(const FRandomStream InRandomStream, const int32 GenotypeSize, const int32 InInputSize, const int32 InHiddenSize, const int32 InOutputSize);

    /** Construct Individual with a predefined genotype */
    void Construct(const FRandomStream InRandomStream, const int32 InSize, const TArray<float>& InGenotype);

    /** Get Fitness */
    UFUNCTION()
    float GetFitness() const;

    /** Set Fitness */
    UFUNCTION()
    void SetFitness(const float InFitness);

    /** Get Size */
    UFUNCTION()
    int32 GetSize() const;

    /** Set Size */
    UFUNCTION()
    void SetSize(const int32 InSize);

    /** Get Genotype */
    UFUNCTION()
    TArray<float> GetGenotype() const;

    /** Get mutable genotype */
    UFUNCTION()
    TArray<float>& GetMutableGenotype();

    /** Set Genotype */
    UFUNCTION()
    void SetGenotype(const TArray<float>& InSize);

    /** Copy from Individual */
    UFUNCTION()
    void CopyFromIndividual(const UNsNNIndividual* const InIndividual);

    /** Recombine */
    UFUNCTION()
    void Recombine(const UNsNNIndividual* const InIndividual, const float InProbability, const FRandomStream InRandomStream);

    /** Mutation */
    UFUNCTION()
    void Mutation(const float InProbability, const FRandomStream InStream);

// Variables
private:

    /** Genotype */
    UPROPERTY()
    TArray<float> Genotype;

    /** Fitness */
    UPROPERTY()
    float Fitness;
};