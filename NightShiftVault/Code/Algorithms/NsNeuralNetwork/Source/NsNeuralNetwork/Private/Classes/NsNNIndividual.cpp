// Copyright © 2024 mykaa. All rights reserved.

#include "Classes/NsNNIndividual.h"

#include "Classes/NsNNGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/NsNNFunctionLibrary.h"

UNsNNIndividual::UNsNNIndividual()
    : Size(0)
    , Fitness(0)
{}

void UNsNNIndividual::Construct(const FRandomStream InRandomStream, const int32 InSize)
{
    Size = InSize;
    Genotype.SetNum(Size);

    for (float& Gene : Genotype)
    {
        Gene = UKismetMathLibrary::RandomFloatInRangeFromStream(InRandomStream, -4.f, 4.f);
    }
}

void UNsNNIndividual::Construct(const FRandomStream InRandomStream, const int32 InSize, const TArray<float>& InGenotype)
{
    ensureMsgf(Size == InGenotype.Num(), TEXT("UNsNNIndividual::Construct : Genotype Size does not meet the InSize"));
    Size = InSize;
    Genotype = InGenotype;

    if (const UWorld* const World = GetWorld())
    {
        if (const ANsNNGameMode* const NeuralGameMode = Cast<ANsNNGameMode>(UGameplayStatics::GetGameMode(World)))
        {
            if (UNsNNFunctionLibrary::HasActiveFlag(NeuralGameMode->GetSessionOverride(), ENsTrainSessionOverride::MutateGenotypeInjection))
            {
                Mutation(NeuralGameMode->GetMutationChance(), InRandomStream);
            }
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
    return Size;
}

void UNsNNIndividual::SetSize(const int32 InSize)
{
    Size = InSize;
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
            const int32 PointOfCutOne = UKismetMathLibrary::RandomIntegerInRangeFromStream(InRandomStream, 1, Size - 2);
            const int32 PointOfCutTwo = UKismetMathLibrary::RandomIntegerInRangeFromStream(InRandomStream, PointOfCutOne + 1, Size - 1);

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