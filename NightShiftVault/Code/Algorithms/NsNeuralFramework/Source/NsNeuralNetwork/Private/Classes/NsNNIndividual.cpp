﻿// Copyright (C) 2024 mykaa. All rights reserved.

#include "NsNNIndividual.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/NsNNFunctionLibrary.h"
#include "NsNNSessionSubsystem.h"

UNsNNIndividual::UNsNNIndividual()
    : Fitness(0)
{}

void UNsNNIndividual::Construct(const FRandomStream InRandomStream, const int32 InSize)
{
    Genotype.SetNum(InSize);

    for (float& Gene : Genotype)
    {
        Gene = UKismetMathLibrary::RandomFloatInRangeFromStream(InRandomStream, -4.f, 4.f);
    }
}

void UNsNNIndividual::Construct(const FRandomStream InRandomStream, const int32 InSize, const TArray<float>& InGenotype)
{
    ensureMsgf(InSize == InGenotype.Num(), TEXT("UNsNNIndividual::Construct : Genotype Size does not meet the InSize"));
    Genotype = InGenotype;

    if (const UNsNNSessionSubsystem* const NeuralSubsystem = UNsNNSessionSubsystem::GetSubsystem())
    {
        if (UNsNNFunctionLibrary::HasActiveFlag(NeuralSubsystem->GetSessionOverride(), ENsTrainSessionOverride::MutateGenotypeInjection))
        {
            Mutation(NeuralSubsystem->GetMutationChance(), InRandomStream);
        }
    }
}

float UNsNNIndividual::GetFitness() const
{
    return Fitness;
}

void UNsNNIndividual::SetFitness(const float InFitness)
{
    Fitness = InFitness;
}

int32 UNsNNIndividual::GetSize() const
{
    return Genotype.Num();
}

void UNsNNIndividual::SetSize(const int32 InSize)
{
    Genotype.SetNum(InSize);
}

TArray<float> UNsNNIndividual::GetGenotype() const
{
    return Genotype;
}

TArray<float>& UNsNNIndividual::GetMutableGenotype()
{
  return Genotype;
}

void UNsNNIndividual::SetGenotype(const TArray<float>& InGenotype)
{
    Genotype = InGenotype;
}

void UNsNNIndividual::CopyFromIndividual(const UNsNNIndividual* const InIndividual)
{
    if (InIndividual != nullptr)
    {
        SetGenotype(InIndividual->GetGenotype());
        SetSize(InIndividual->GetSize());
    }
}

void UNsNNIndividual::Recombine(const UNsNNIndividual* const InIndividual, const float InProbability, const FRandomStream InRandomStream)
{
    if (InIndividual != nullptr)
    {
        const float Recombination = UKismetMathLibrary::RandomFloatFromStream(InRandomStream);

        if (Recombination < InProbability)
        {
            const int32 PointOfCutOne = UKismetMathLibrary::RandomIntegerInRangeFromStream(InRandomStream, 1, Genotype.Num() - 2);
            const int32 PointOfCutTwo = UKismetMathLibrary::RandomIntegerInRangeFromStream(InRandomStream, PointOfCutOne + 1, Genotype.Num() - 1);

            for (int32 i = PointOfCutOne; i < PointOfCutTwo; ++i)
            {
                const float GeneHolder = Genotype[i];
                Genotype[i] = InIndividual->GetGenotype()[i];
                InIndividual->GetGenotype()[i] = GeneHolder;
            }
        }
    }
}

void UNsNNIndividual::Mutation(const float InProbability, const FRandomStream InStream)
{
    for (int32 i = 0; i < Genotype.Num(); ++i)
    {
        const float Mutation = UKismetMathLibrary::RandomFloatFromStream(InStream);
        if (Mutation < InProbability)
        {
            // Apply mutation modifier with a controlled random value
            const float MutationModifier = UKismetMathLibrary::RandomFloatInRangeFromStream(InStream, -0.5f, 0.5f);
            Genotype[i] += MutationModifier;
        }
    }
}