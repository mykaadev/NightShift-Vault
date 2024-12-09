// Copyright © 2024 mykaa. All rights reserved.

#include "Classes/NsNNGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Classes/Networks/NsNNTwoLayerFeedForward.h"
#include "Classes/NsNNIndividual.h"
#include "Classes/NsNNSettings.h"
#include "Classes/NsNNTrainController.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/NsNNFunctionLibrary.h"
#include "UI/NsNNControlPanel.h"

ANsNNGameMode::ANsNNGameMode()
  : RandomSeed(0)
  , Generation(0)
  , BestIndividual(nullptr)
  , CurrentIndividualIndex(0)
  , AverageFitness(0)
  , FitnessSum(0)
  , BestFitnessInGeneration(0)
  , BestFitnessInSession(0)
  , BestFitnessRun(0)
  , BestIndividualIDFromSession(0)
  , BestGenerationIDFromSession(0)
  , IndividualTime(0)
  , bCurrentIndividualFinished(false)
  , bIsTraining(false)
  , SessionOverrideFlags(0)
{}

void ANsNNGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (const UWorld* const World = GetWorld())
    {
        if (const UNsNNSettings* const Settings = GetDefault<UNsNNSettings>())
        {
            if (UClass* const ControlPanel = Settings->ControlPanel.LoadSynchronous())
            {
                if (UNsNNControlPanel* const NNControlPanel = CreateWidget<UNsNNControlPanel>(World->GetFirstPlayerController(), ControlPanel))
                {
                    NNControlPanel->AddToViewport();
                }
            }
        }
    }
}

void ANsNNGameMode::SetRandomSeed(const int32 InRandomSeed)
{
    RandomSeed = InRandomSeed;
}

void ANsNNGameMode::SetPopulationParameters(const int32 InPopulationSize, const int32 InMaxTimePerIndividual)
{
    SessionData.Population.PopulationSize = InPopulationSize;
    SessionData.Population.MaxTimePerIndividual = InMaxTimePerIndividual;
}

void ANsNNGameMode::SetMutationAndRecombinationParameters(const float InMutationChance, const float InRecombinationChance)
{
    SessionData.Population.MutationChance = InMutationChance;
    SessionData.Population.RecombinationChance = InRecombinationChance;
}

void ANsNNGameMode::SetNeuralNetworkStructure(const int32 InNeuralInputs, const int32 InNeuralHiddenLayerSize, const int32 InNeuralOutputs)
{
    SessionData.Agent.NeuralInputs = InNeuralInputs;
    SessionData.Agent.NeuralHiddenLayerSize = InNeuralHiddenLayerSize;
    SessionData.Agent.NeuralOutputs = InNeuralOutputs;
}

void ANsNNGameMode::SetSessionData(const FNsTrainSessionSetup& InSessionData)
{
    SessionData = InSessionData;
    if (SessionData.NeuralNetwork != nullptr && SessionData.Agent.Controller != nullptr)
    {
        SetNeuralNetwork(NewObject<UNsNNBaseNetwork>(this, SessionData.NeuralNetwork, NAME_None));
    }
}

int32 ANsNNGameMode::GetInputSize() const
{
    return SessionData.Agent.NeuralInputs;
}

UNsNNBaseNetwork* ANsNNGameMode::GetNeuralNetwork() const
{
    return NeuralNetwork;
}

void ANsNNGameMode::SetNeuralNetwork(UNsNNBaseNetwork* const InNetwork)
{
    if (InNetwork != nullptr)
    {
        NeuralNetwork = InNetwork;
    }
}

ANsNNTrainController* ANsNNGameMode::GetAgentController() const
{
    return CurrentController;
}

void ANsNNGameMode::SetAgentController(ANsNNTrainController* const InController)
{
    if (InController != nullptr)
    {
        CurrentController = InController;
    }
}

void ANsNNGameMode::OnStartRequestReceived()
{
    // Set up the random stream with the base seed
    UKismetMathLibrary::SetRandomStreamSeed(RandomStream, RandomSeed);
    UKismetMathLibrary::ResetRandomStream(RandomStream);

    if (const APawn* const Agent = UNsNNFunctionLibrary::SpawnAgentFromClass(this, SessionData.Agent.Pawn, SessionData.Agent.Controller, SessionData.Gym.InitialSpawnLocation, SessionData.Gym.InitialSpawnRotation, nullptr))
    {
        if (ANsNNTrainController* const TrainController = Cast<ANsNNTrainController>(Agent->GetController()))
        {
            SetAgentController(TrainController);
            if (NeuralNetwork != nullptr && CurrentController != nullptr)
            {
                const int32 IndividualSize = NeuralNetwork->Initialize(SessionData.Agent.NeuralInputs
                                                                     , SessionData.Agent.NeuralHiddenLayerSize
                                                                     , SessionData.Agent.NeuralOutputs);
                InitializePopulation(IndividualSize);
                SetDefaultSessionValues();
                StartTrainSession();
            }
        }
    }
}

void ANsNNGameMode::InitializePopulation(const int32 InIndividualSize)
{
    CurrentPopulation.Empty();
    CurrentPopulation.SetNum(SessionData.Population.PopulationSize);

    if (UNsNNFunctionLibrary::HasActiveFlag(SessionOverrideFlags, ENsTrainSessionOverride::InjectPopulation))
    {
        // #NsNeuralNetworks #NsToDo : Parse population file
        // ensure population size meets the session parameters
        // if not, change the session parameters to match the population data

        if (UNsNNFunctionLibrary::HasActiveFlag(SessionOverrideFlags, ENsTrainSessionOverride::MutatePopulationInjection))
        {
            // Construct each individual with a predefined genotype and mutate it
        }
        else
        {
            // Construct each individual with a predefined genotype
        }
    }
    else
    {
        for (int32 i = 0; i < CurrentPopulation.Num(); ++i)
        {
            CurrentPopulation[i] = NewObject<UNsNNIndividual>(this, UNsNNIndividual::StaticClass(), NAME_None);
            if (CurrentPopulation.IsValidIndex(i) && CurrentPopulation[i] != nullptr)
            {
                // Derive a unique seed for each individual
                FRandomStream IndividualRandomStream;
                IndividualRandomStream.Initialize(RandomSeed + Generation + i); // Unique seed using base seed, generation, and individual index

                if (UNsNNFunctionLibrary::HasActiveFlag(SessionOverrideFlags, ENsTrainSessionOverride::InjectGenotype))
                {
                    CurrentPopulation[i]->Construct(IndividualRandomStream, InIndividualSize, SessionOverrideGenotype);
                }
                else
                {
                    CurrentPopulation[i]->Construct(IndividualRandomStream, InIndividualSize);
                }
            }
        }
    }
}

void ANsNNGameMode::SetDefaultSessionValues()
{
    CurrentIndividualIndex = INDEX_NONE;
    AverageFitness = 0.f;
    FitnessSum = 0.f;
    IndividualTime = 0;
    bCurrentIndividualFinished = true;
    bIsTraining = true;
}

void ANsNNGameMode::StartTrainSession()
{
    if (const UWorld* const World = GetWorld())
    {
        World->GetTimerManager().SetTimer(HearthBeatTimerHandle, this, &ANsNNGameMode::OnHeartBeat, 1.f, true, 1.f);
    }
}

void ANsNNGameMode::OnHeartBeat()
{
    if (bIsTraining)
    {
        ++IndividualTime;

        if (IndividualTime >= SessionData.Population.MaxTimePerIndividual)
        {
            EndCurrentEvaluation();
        }
        else
        {
            if (bCurrentIndividualFinished && CurrentController != nullptr && NeuralNetwork != nullptr)
            {
                CurrentController->OnResetRequested();
                ++CurrentIndividualIndex;

                if (CurrentIndividualIndex == SessionData.Population.PopulationSize)
                {
                    NextGeneration();
                }
                else if (CurrentPopulation.IsValidIndex(CurrentIndividualIndex))
                {
                    NeuralNetwork->SetWeights(CurrentPopulation[CurrentIndividualIndex]->GetGenotype());
                    bCurrentIndividualFinished = false;
                }
            }
        }
    }
}

void ANsNNGameMode::EndCurrentEvaluation()
{
    if (bIsTraining && CurrentPopulation.IsValidIndex(CurrentIndividualIndex))
    {
        const float Fitness = ComputeFitness();
        FitnessSum += Fitness;

        if (UNsNNIndividual* const CurrentIndividual = CurrentPopulation[CurrentIndividualIndex])
        {
            CurrentIndividual->SetFitness(Fitness);

            if (Fitness > BestFitnessInGeneration)
            {
                BestFitnessInGeneration = Fitness;
                BestIndividual = CurrentIndividual;

                if (BestFitnessInGeneration > BestFitnessInSession)
                {
                    BestFitnessInSession = BestFitnessInGeneration;
                    BestIndividualIDFromSession = CurrentIndividualIndex;
                    BestGenerationIDFromSession = Generation;
                    BestGenotypeFromSession = CurrentIndividual->GetGenotype();
                }
            }

            IndividualTime = 0;
            bCurrentIndividualFinished = true;
        }
    }
}

float ANsNNGameMode::ComputeFitness() const
{
    if (CurrentController != nullptr)
    {
        return CurrentController->ComputeFitness();
    }

    // this can never fail otherwise, F
    return -1.f;
}

void ANsNNGameMode::NextGeneration()
{
    AverageFitness = FitnessSum / static_cast<float>(SessionData.Population.PopulationSize);
    BestFitnessRun = BestFitnessInGeneration;

    FitnessSum = 0;
    CurrentIndividualIndex = INDEX_NONE;

    NaturalSelection();
    Recombine();
    Mutate();

    if (UNsNNIndividual* const BestDude = NewObject<UNsNNIndividual>(this, UNsNNIndividual::StaticClass(), NAME_None))
    {
        BestDude->CopyFromIndividual(BestIndividual);
        NextPopulation.Emplace(BestDude);
        CurrentPopulation = NextPopulation;

        ++Generation;

        bCurrentIndividualFinished = true;
        BestFitnessInGeneration = 0.f;
    }
}

void ANsNNGameMode::NaturalSelection()
{
    NextPopulation.Empty();

    for (int32 i = 0; i < SessionData.Population.PopulationSize; ++i)
    {
        const int32 ChosenIndexOne = UKismetMathLibrary::RandomIntegerFromStream(RandomStream, SessionData.Population.PopulationSize);
        const int32 ChosenIndexTwo = UKismetMathLibrary::RandomIntegerFromStream(RandomStream, SessionData.Population.PopulationSize);
        const UNsNNIndividual* const IndividualOne = CurrentPopulation[ChosenIndexOne];
        const UNsNNIndividual* const IndividualTwo = CurrentPopulation[ChosenIndexTwo];

        if (UNsNNIndividual* const Chosen = NewObject<UNsNNIndividual>(this, UNsNNIndividual::StaticClass(), NAME_None))
        {
            Chosen->CopyFromIndividual(IndividualOne->GetFitness() >= IndividualTwo->GetFitness() ? IndividualOne : IndividualTwo);
            NextPopulation.Emplace(Chosen);
        }
    }
}

void ANsNNGameMode::Recombine()
{
    const int32 LastIndex = FMath::TruncToInt32(static_cast<float>(NextPopulation.Num() / 2.f) - 1);

    for (int32 i = 0; i < LastIndex; ++i)
    {
        const int32 IndexToRecombine = i * 2;
        if (NextPopulation.IsValidIndex(IndexToRecombine))
        {
            NextPopulation[IndexToRecombine]->Recombine(NextPopulation[IndexToRecombine + 1], SessionData.Population.RecombinationChance, RandomStream);
        }
    }
}

void ANsNNGameMode::Mutate()
{
    for (UNsNNIndividual* const Individual : CurrentPopulation)
    {
        if (Individual != nullptr)
        {
            Individual->Mutation(SessionData.Population.MutationChance, RandomStream);
        }
    }
}

void ANsNNGameMode::StopTrainSession()
{
    if (const UWorld* const World = GetWorld())
    {
        if (World->GetTimerManager().TimerExists(HearthBeatTimerHandle))
        {
            if (CurrentController != nullptr)
            {
                bIsTraining = false;

                OnHeartBeat();

                if (AActor* const Entity = Cast<AActor>(CurrentController->GetOwner()))
                {
                    Entity->Destroy();
                    World->GetTimerManager().ClearTimer(HearthBeatTimerHandle);
                }
            }
        }
    }
}

void ANsNNGameMode::SaveBestGenotypeFromSession()
{
    if (!BestGenotypeFromSession.IsEmpty())
    {
        SaveSessionData(BestGenotypeFromSession, !SessionData.Name.IsEmpty() ? SessionData.Name : TEXT("Default"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No Best Individual available to save."));
    }
}

void ANsNNGameMode::SaveCurrentGenotype()
{
    if (CurrentPopulation.IsValidIndex(CurrentIndividualIndex) && CurrentPopulation[CurrentIndividualIndex] != nullptr)
    {
        SaveSessionData(CurrentPopulation[CurrentIndividualIndex]->GetGenotype(), !SessionData.Name.IsEmpty() ? SessionData.Name : TEXT("Default"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Current Individual wasn't available to save."));
    }
}

void ANsNNGameMode::SaveSessionData(const TArray<float>& InGenotype, const FString& InFileNameSuffix)
{
    if (const UNsNNSettings* const Settings = GetDefault<UNsNNSettings>())
    {
        FString BasePath = Settings->DataExportPath;

        if (!FPaths::DirectoryExists(BasePath) || BasePath.IsEmpty())
        {
            if (!FPaths::DirectoryExists(BasePath))
            {
                IFileManager::Get().MakeDirectory(*BasePath, true);
            }
            else if (BasePath.IsEmpty())
            {
                if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
                {
                    const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
                    FString SelectedPath;
                    if (DesktopPlatform->OpenDirectoryDialog(ParentWindowHandle, TEXT("Select Save Location"), FPaths::ProjectDir(), SelectedPath))
                    {
                        BasePath = SelectedPath;
                        UE_LOG(LogTemp, Log, TEXT("User selected path: %s"), *BasePath);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("User canceled the directory selection. Aborting"));
                        return;
                    }
                }
            }
        }

        const FString& FilePath = FPaths::Combine(BasePath, FString::Printf(TEXT("%s.txt"), *InFileNameSuffix));
        FString FileContents;

        // SESSION DATA
        FileContents += TEXT("[SESSION DATA]\n");
        FileContents += FString::Printf(TEXT("RandomSeed=%d\n"), RandomSeed);
        FileContents += FString::Printf(TEXT("PopulationSize=%d\n"), SessionData.Population.PopulationSize);
        FileContents += FString::Printf(TEXT("MaxTimePerIndividual=%d\n"), SessionData.Population.MaxTimePerIndividual);
        FileContents += FString::Printf(TEXT("MutationChance=%f\n"), SessionData.Population.MutationChance);
        FileContents += FString::Printf(TEXT("RecombinationChance=%f\n"), SessionData.Population.RecombinationChance);
        FileContents += FString::Printf(TEXT("NeuralInputs=%d\n"), SessionData.Agent.NeuralInputs);
        FileContents += FString::Printf(TEXT("NeuralHiddenLayerSize=%d\n"), SessionData.Agent.NeuralHiddenLayerSize);
        FileContents += FString::Printf(TEXT("NeuralOutputs=%d\n"), SessionData.Agent.NeuralOutputs);

        if (CurrentController != nullptr)
        {
            FileContents += FString::Printf(TEXT("Controller=%s\n"), *CurrentController->GetName());
        }

        // AGENT DATA
        FileContents += TEXT("\n[AGENT DATA]\n");
        FileContents += TEXT("Genotype=");
        FileContents += UNsNNFunctionLibrary::CompressGenotype(InGenotype);
        FileContents += TEXT("\n");

        // POPULATION DATA
        FileContents += TEXT("\n[POPULATION DATA]\n");
        FileContents += FString::Printf(TEXT("PopulationSize=%d\n"), SessionData.Population.PopulationSize);
        FileContents += TEXT("Genotypes=\n");
        for (int32 i = 0; i < CurrentPopulation.Num(); ++i)
        {
            if (CurrentPopulation[i] != nullptr)
            {
                FileContents += FString::Printf(TEXT("Agent_%d="), i);
                FileContents += UNsNNFunctionLibrary::CompressGenotype(CurrentPopulation[i]->GetGenotype());
                FileContents += TEXT("\n");
            }
        }

        // Write to file
        if (FFileHelper::SaveStringToFile(FileContents, *FilePath))
        {
            UE_LOG(LogTemp, Log, TEXT("Saved file to %s"), *FilePath);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to save file to %s"), *FilePath);
        }
    }
}

float ANsNNGameMode::GetMutationChance() const
{
    return SessionData.Population.MutationChance;
}

int32 ANsNNGameMode::GetMaxTimePerIndividual() const
{
    return SessionData.Population.MaxTimePerIndividual;
}

int32 ANsNNGameMode::GetCurrentTimePerIndividual() const
{
    return IndividualTime;
}

int32 ANsNNGameMode::GetCurrentIndividualID() const
{
    return CurrentIndividualIndex;
}

int32 ANsNNGameMode::GetCurrentPopulationID() const
{
    return Generation;
}

int32 ANsNNGameMode::GetCurrentPopulationSize() const
{
    return SessionData.Population.PopulationSize;
}

float ANsNNGameMode::GetCurrentIndividualFitness() const
{
    if (CurrentController != nullptr)
    {
        return CurrentController->ComputeFitness();
    }

    return 0.f;
}

float ANsNNGameMode::GetBestFitnessInSession() const
{
    return BestFitnessInSession;
}

float ANsNNGameMode::GetBestFitnessInPopulation() const
{
    return BestFitnessInGeneration;
}

int32 ANsNNGameMode::GetBestIndividualIDInSession() const
{
    return BestIndividualIDFromSession;
}

float ANsNNGameMode::GetBestPopulationIDInSession() const
{
    return BestGenerationIDFromSession;
}

uint8 ANsNNGameMode::GetSessionOverride() const
{
    return SessionOverrideFlags;
}

uint8& ANsNNGameMode::GetMutableSessionOverride()
{
    return SessionOverrideFlags;
}

FNsTrainSessionSetup ANsNNGameMode::GetSessionData() const
{
    return SessionData;
}

FNsTrainSessionSetup& ANsNNGameMode::GetMutableSessionData()
{
    return SessionData;
}
