// Copyright (C) 2024 mykaa. All rights reserved.

#include "NsNNSessionSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Kismet/KismetMathLibrary.h"
#include "Libraries/NsNNFunctionLibrary.h"
#include "Networks/NsNNArchitecture.h"
#include "NsNNIndividual.h"
#include "NsNNSettings.h"
#include "NsNNTrainController.h"
#include "UI/NsNNControlPanel.h"

UNsNNSessionSubsystem::UNsNNSessionSubsystem()
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
{
}

UNsNNSessionSubsystem* UNsNNSessionSubsystem::GetSubsystem()
{
    UNsNNSessionSubsystem* ToReturn = nullptr;

    if (GEngine != nullptr)
    {
        // Iterate through World Contexts until a Game World is found
        for (const FWorldContext& Context : GEngine->GetWorldContexts())
        {
            if (const UWorld* const World = Context.World())
            {
                if (World->IsGameWorld() || World->IsPlayInEditor())
                {
                    if (UNsNNSessionSubsystem* const SessionSubsystem = UGameInstance::GetSubsystem<UNsNNSessionSubsystem>(World->GetGameInstance()))
                    {
                        ToReturn = SessionSubsystem;
                        break;
                    }
                }
            }
        }
    }

    return ToReturn;
}

UNsNNControlPanel* UNsNNSessionSubsystem::GetControlPanel() const
{
    return ControlPanel;
}

void UNsNNSessionSubsystem::Init()
{
    if (UWorld* const World = GetWorld())
    {
        if (const UNsNNSettings* const Settings = GetDefault<UNsNNSettings>())
        {
            if (UClass* const ControlPanelClass = Settings->ControlPanel.LoadSynchronous())
            {
                ControlPanel = CreateWidget<UNsNNControlPanel>(World, ControlPanelClass);
                if (ControlPanel != nullptr)
                {
                    ControlPanel->AddToViewport();
                    if (APlayerController* const PlayerController = World->GetFirstPlayerController())
                    {
                        PlayerController->bShowMouseCursor = true;
                    }
                }
            }
        }
    }
}

void UNsNNSessionSubsystem::SetRandomSeed(const int32 InRandomSeed)
{
    RandomSeed = InRandomSeed;
}

void UNsNNSessionSubsystem::SetPopulationParameters(const int32 InPopulationSize, const int32 InMaxTimePerIndividual)
{
    SessionData.Population.PopulationSize = InPopulationSize;
    SessionData.Population.MaxTimePerIndividual = InMaxTimePerIndividual;
}

void UNsNNSessionSubsystem::SetMutationAndRecombinationParameters(const float InMutationChance, const float InRecombinationChance)
{
    SessionData.Population.MutationChance = InMutationChance;
    SessionData.Population.RecombinationChance = InRecombinationChance;
}

void UNsNNSessionSubsystem::SetNeuralNetworkStructure(const int32 InNeuralInputs, const int32 InNeuralHiddenLayerSize, const int32 InNeuralOutputs)
{
    SessionData.Agent.NeuralInputs = InNeuralInputs;
    SessionData.Agent.NeuralHiddenLayerSize = InNeuralHiddenLayerSize;
    SessionData.Agent.NeuralOutputs = InNeuralOutputs;
}

void UNsNNSessionSubsystem::SetNeuralNetworkRegulators(const float InLearningRate, const float InDropout, const float InL2)
{
    SessionData.Agent.LearningRate = InLearningRate;
    SessionData.Agent.DropoutRate = InDropout;
    SessionData.Agent.L2RegulatorStrength = InL2;
}

void UNsNNSessionSubsystem::SetSessionData(const FNsTrainSessionSetup& InSessionData)
{
    SessionData = InSessionData;
    if (SessionData.NeuralNetwork != nullptr && SessionData.Agent.Controller != nullptr)
    {
        SetNeuralNetwork(NewObject<UNsNNArchitecture>(this, SessionData.NeuralNetwork.LoadSynchronous(), NAME_None));
    }
}

int32 UNsNNSessionSubsystem::GetInputSize() const
{
    return SessionData.Agent.NeuralInputs;
}

UNsNNArchitecture* UNsNNSessionSubsystem::GetNeuralNetwork() const
{
    return NeuralNetwork;
}

void UNsNNSessionSubsystem::SetNeuralNetwork(UNsNNArchitecture* const InNetwork)
{
    if (InNetwork != nullptr)
    {
        NeuralNetwork = InNetwork;
    }
}

ANsNNTrainController* UNsNNSessionSubsystem::GetAgentController() const
{
    return CurrentController;
}

void UNsNNSessionSubsystem::SetAgentController(ANsNNTrainController* const InController)
{
    if (InController != nullptr)
    {
        CurrentController = InController;
    }
}

void UNsNNSessionSubsystem::OnStartRequestReceived()
{
    // Set up the random stream with the base seed
    UKismetMathLibrary::SetRandomStreamSeed(RandomStream, RandomSeed);
    UKismetMathLibrary::ResetRandomStream(RandomStream);

    SetDefaultSessionValues();

    // Initialize Neural Network and Population
    if (NeuralNetwork != nullptr)
    {
        const FNsTrainAgentSetup AgentData = SessionData.Agent;
        const int32 GenotypeSize = NeuralNetwork->InitializeNetwork(AgentData.NeuralInputs, AgentData.NeuralHiddenLayerSize, AgentData.NeuralOutputs);
        NeuralNetwork->InitializeRegulators(AgentData.LearningRate, AgentData.DropoutRate, AgentData.L2RegulatorStrength);

        InitializePopulation(GenotypeSize);

        // Initialize our agent
        if (const APawn* const Agent = UNsNNFunctionLibrary::SpawnAndPocessAgent(this, SessionData.Agent.Pawn.LoadSynchronous(), SessionData.Agent.Controller.LoadSynchronous(), SessionData.Gym.InitialSpawnLocation, SessionData.Gym.InitialSpawnRotation, nullptr))
        {
            CurrentController = Cast<ANsNNTrainController>(Agent->GetController());
            if (CurrentController != nullptr && CurrentPopulation.IsValidIndex(0))
            {
                CurrentController->Genotype = CurrentPopulation[0]->GetGenotype();
                CurrentController->Initialize();
                StartTrainSession();
            }
        }
    }
}

void UNsNNSessionSubsystem::InitializePopulation(const int32 InIndividualSize)
{
    CurrentPopulation.Empty();
    CurrentPopulation.SetNum(SessionData.Population.PopulationSize);
    for (int32 i = 0; i < CurrentPopulation.Num(); ++i)
    {
        CurrentPopulation[i] = NewObject<UNsNNIndividual>(this, UNsNNIndividual::StaticClass(), NAME_None);
        if (CurrentPopulation[i] != nullptr)
        {
            // Derive a unique seed for each individual
            FRandomStream IndividualRandomStream;
            IndividualRandomStream.Initialize(RandomSeed + Generation + i); // Unique seed using base seed, generation, and individual index
            CurrentPopulation[i]->Construct(IndividualRandomStream, InIndividualSize, SessionData.Agent.NeuralInputs, SessionData.Agent.NeuralHiddenLayerSize, SessionData.Agent.NeuralOutputs);
        }
    }
}

void UNsNNSessionSubsystem::SetDefaultSessionValues()
{
    CurrentIndividualIndex = INDEX_NONE;
    AverageFitness = 0.f;
    FitnessSum = 0.f;
    IndividualTime = 0;
    bCurrentIndividualFinished = true;
    bIsTraining = true;
}

void UNsNNSessionSubsystem::StartTrainSession()
{
    if (const UWorld* const World = GetWorld())
    {
        World->GetTimerManager().SetTimer(HeartBeatTimerHandle, this, &UNsNNSessionSubsystem::OnHeartBeat, 1.f, true, 1.f);
    }
}

void UNsNNSessionSubsystem::OnHeartBeat()
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
                else if (CurrentPopulation.IsValidIndex(CurrentIndividualIndex) && CurrentPopulation[CurrentIndividualIndex] != nullptr)
                {
                    NeuralNetwork->SetNetworkWeights(CurrentPopulation[CurrentIndividualIndex]->GetGenotype());
                    bCurrentIndividualFinished = false;
                }
            }
        }
    }
}

void UNsNNSessionSubsystem::EndCurrentEvaluation()
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

float UNsNNSessionSubsystem::ComputeFitness() const
{
    if (CurrentController != nullptr)
    {
        return CurrentController->ComputeFitness();
    }

    // this can never fail otherwise, F
    return -1.f;
}

void UNsNNSessionSubsystem::NextGeneration()
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

void UNsNNSessionSubsystem::NaturalSelection()
{
    NextPopulation.Empty();
    NextPopulation.Reserve(SessionData.Population.PopulationSize);
    for (int32 i = 0; i < SessionData.Population.PopulationSize; ++i)
    {
        const int32 ChosenIndexOne = UKismetMathLibrary::RandomIntegerFromStream(RandomStream, SessionData.Population.PopulationSize);
        const int32 ChosenIndexTwo = UKismetMathLibrary::RandomIntegerFromStream(RandomStream, SessionData.Population.PopulationSize);
        const UNsNNIndividual* const IndividualOne = CurrentPopulation[ChosenIndexOne];
        const UNsNNIndividual* const IndividualTwo = CurrentPopulation[ChosenIndexTwo];

        if (IndividualOne != nullptr && IndividualTwo != nullptr)
        {
            if (UNsNNIndividual* const Chosen = NewObject<UNsNNIndividual>(this, UNsNNIndividual::StaticClass(), NAME_None))
            {
                Chosen->CopyFromIndividual(IndividualOne->GetFitness() >= IndividualTwo->GetFitness() ? IndividualOne : IndividualTwo);
                NextPopulation.Emplace(Chosen);
            }
        }
    }
}

void UNsNNSessionSubsystem::Recombine()
{
    const int32 LastIndex = FMath::TruncToInt32(static_cast<float>(NextPopulation.Num() * 0.5f) - 1.f);

    for (int32 i = 0; i < LastIndex; ++i)
    {
        const int32 IndexToRecombine = i * 2;
        if (NextPopulation.IsValidIndex(IndexToRecombine) && NextPopulation[IndexToRecombine] != nullptr)
        {
            NextPopulation[IndexToRecombine]->Recombine(NextPopulation[IndexToRecombine + 1], SessionData.Population.RecombinationChance, RandomStream);
        }
    }
}

void UNsNNSessionSubsystem::Mutate()
{
    for (UNsNNIndividual* const Individual : CurrentPopulation)
    {
        if (Individual != nullptr)
        {
            Individual->Mutation(SessionData.Population.MutationChance, RandomStream);
        }
    }
}

void UNsNNSessionSubsystem::StopTrainSession()
{
    if (const UWorld* const World = GetWorld())
    {
        if (World->GetTimerManager().TimerExists(HeartBeatTimerHandle))
        {
            if (CurrentController != nullptr)
            {
                bIsTraining = false;

                OnHeartBeat();

                if (AActor* const Entity = Cast<AActor>(CurrentController->GetOwner()))
                {
                    Entity->Destroy();
                    World->GetTimerManager().ClearTimer(HeartBeatTimerHandle);
                }
            }
        }
    }
}

void UNsNNSessionSubsystem::SaveBestGenotypeFromSession()
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

void UNsNNSessionSubsystem::SaveCurrentGenotype()
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

void UNsNNSessionSubsystem::SaveSessionData(const TArray<float>& InGenotype, const FString& InFileNameSuffix)
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
                if (IDesktopPlatform* const DesktopPlatform = FDesktopPlatformModule::Get())
                {
                    const void* const ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
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

        // Save Session Data
        FileContents += TEXT("[SESSION DATA]\n");
        FileContents += FString::Printf(TEXT("RandomSeed=%d\n"), RandomSeed);
        FileContents += FString::Printf(TEXT("NeuralInputs=%d\n"), SessionData.Agent.NeuralInputs);
        FileContents += FString::Printf(TEXT("NeuralHiddenLayerSize=%d\n"), SessionData.Agent.NeuralHiddenLayerSize);
        FileContents += FString::Printf(TEXT("NeuralOutputs=%d\n"), SessionData.Agent.NeuralOutputs);
        FileContents += FString::Printf(TEXT("LearningRate=%f\n"), SessionData.Agent.LearningRate);
        FileContents += FString::Printf(TEXT("DropoutRate=%f\n"), SessionData.Agent.DropoutRate);
        FileContents += FString::Printf(TEXT("L2Regulator=%f\n"), SessionData.Agent.L2RegulatorStrength);

        // Save Agent Data
        FileContents += TEXT("\n[AGENT DATA]\n");
        if (SessionData.Agent.Controller.IsValid())
        {
            const FSoftClassPath ControllerPath(SessionData.Agent.Controller.Get());
            FileContents += FString::Printf(TEXT("Controller=%s\n"), *ControllerPath.ToString());
        }

        if (SessionData.NeuralNetwork.IsValid())
        {
            const FSoftClassPath NeuralNetworkPath(SessionData.NeuralNetwork.Get());
            FileContents += FString::Printf(TEXT("NeuralNetwork=%s\n"), *NeuralNetworkPath.ToString());
        }
        FileContents += TEXT("Genotype=");
        FileContents += UNsNNFunctionLibrary::CompressGenotype(InGenotype);
        FileContents += TEXT("\n");

        // Save Population Data
        FileContents += TEXT("\n[POPULATION DATA]\n");
        FileContents += FString::Printf(TEXT("MutationChance=%f\n"), SessionData.Population.MutationChance);
        FileContents += FString::Printf(TEXT("RecombinationChance=%f\n"), SessionData.Population.RecombinationChance);
        FileContents += FString::Printf(TEXT("PopulationSize=%d\n"), SessionData.Population.PopulationSize);
        FileContents += FString::Printf(TEXT("MaxTimePerIndividual=%d\n"), SessionData.Population.MaxTimePerIndividual);
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

bool UNsNNSessionSubsystem::LoadSessionDataFromFile()
{
    TMap<FString, FString> InOutParsedData;
    if (UNsNNFunctionLibrary::LoadAndParseTrainFileData(InOutParsedData, TEXT("=")))
    {
        // Assign Session variables
        RandomSeed = UNsNNFunctionLibrary::GetValueFromTrainData<int32>(InOutParsedData, TEXT("RandomSeed"));
        SessionData.Agent.NeuralInputs = UNsNNFunctionLibrary::GetValueFromTrainData<int32>(InOutParsedData, TEXT("NeuralInputs"));
        SessionData.Agent.NeuralHiddenLayerSize = UNsNNFunctionLibrary::GetValueFromTrainData<int32>(InOutParsedData, TEXT("NeuralHiddenLayerSize"));
        SessionData.Agent.NeuralOutputs = UNsNNFunctionLibrary::GetValueFromTrainData<int32>(InOutParsedData, TEXT("NeuralOutputs"));

        SessionData.Agent.LearningRate = UNsNNFunctionLibrary::GetValueFromTrainData<float>(InOutParsedData, TEXT("LearningRate"));
        SessionData.Agent.DropoutRate = UNsNNFunctionLibrary::GetValueFromTrainData<float>(InOutParsedData, TEXT("DropoutRate"));
        SessionData.Agent.L2RegulatorStrength = UNsNNFunctionLibrary::GetValueFromTrainData<float>(InOutParsedData, TEXT("L2Regulator"));

        // Assign Agent variables
        if (CurrentController != nullptr)
        {
            APawn* const PlayerPawn = CurrentController->GetPawn();
            CurrentController->Destroy(); // Destroy the old controller since we will create a new one
            CurrentController = UNsNNFunctionLibrary::GetValueFromTrainData<ANsNNTrainController*>(InOutParsedData, TEXT("Controller"));
            CurrentController = Cast<ANsNNTrainController>(UNsNNFunctionLibrary::ChangeControllerForAgent(CurrentController.GetClass(), PlayerPawn, GetWorld()));
            CurrentController->Genotype = UNsNNFunctionLibrary::DecompressGenotype(UNsNNFunctionLibrary::GetValueFromTrainData<FString>(InOutParsedData, TEXT("Genotype")));
        }

        NeuralNetwork = UNsNNFunctionLibrary::GetValueFromTrainData<UNsNNArchitecture*>(InOutParsedData, TEXT("NeuralNetwork"));
        if (NeuralNetwork != nullptr && CurrentController != nullptr)
        {
             const int32 AgentGenotypeSize = NeuralNetwork->InitializeNetwork(SessionData.Agent.NeuralInputs
                                                                        , SessionData.Agent.NeuralHiddenLayerSize
                                                                        , SessionData.Agent.NeuralOutputs);

            CurrentController->Initialize();

            // Assign Population variables
            SessionData.Population.MutationChance = UNsNNFunctionLibrary::GetValueFromTrainData<float>(InOutParsedData, TEXT("MutationChance"));
            SessionData.Population.RecombinationChance = UNsNNFunctionLibrary::GetValueFromTrainData<float>(InOutParsedData, TEXT("RecombinationChance"));
            SessionData.Population.PopulationSize = UNsNNFunctionLibrary::GetValueFromTrainData<int32>(InOutParsedData, TEXT("PopulationSize"));

            CurrentPopulation.Reset(SessionData.Population.PopulationSize);
            InitializePopulation(AgentGenotypeSize);

            for (int32 i = 0; i < SessionData.Population.PopulationSize; ++i)
            {
                const FString Key = FString::Printf(TEXT("Agent_%d"), i);
                if (InOutParsedData.Contains(Key) && CurrentPopulation.IsValidIndex(i) && CurrentPopulation[i] != nullptr)
                {
                    CurrentPopulation[i]->SetGenotype(UNsNNFunctionLibrary::DecompressGenotype(InOutParsedData[Key]));
                }
            }

            CurrentIndividualIndex = 0; // Fully reset the generation
            SessionData.Population.MaxTimePerIndividual = UNsNNFunctionLibrary::GetValueFromTrainData<int32>(InOutParsedData, TEXT("MaxTimePerIndividual"));
        }
    }

    return true;
}

float UNsNNSessionSubsystem::GetMutationChance() const
{
    return SessionData.Population.MutationChance;
}

int32 UNsNNSessionSubsystem::GetMaxTimePerIndividual() const
{
    return SessionData.Population.MaxTimePerIndividual;
}

int32 UNsNNSessionSubsystem::GetCurrentTimePerIndividual() const
{
    return IndividualTime;
}

int32 UNsNNSessionSubsystem::GetCurrentIndividualID() const
{
    return CurrentIndividualIndex;
}

int32 UNsNNSessionSubsystem::GetCurrentPopulationID() const
{
    return Generation;
}

int32 UNsNNSessionSubsystem::GetCurrentPopulationSize() const
{
    return SessionData.Population.PopulationSize;
}

float UNsNNSessionSubsystem::GetCurrentIndividualFitness() const
{
    if (CurrentController != nullptr)
    {
        return CurrentController->ComputeFitness();
    }

    return 0.f;
}

float UNsNNSessionSubsystem::GetBestFitnessInSession() const
{
    return BestFitnessInSession;
}

float UNsNNSessionSubsystem::GetBestFitnessInPopulation() const
{
    return BestFitnessInGeneration;
}

int32 UNsNNSessionSubsystem::GetBestIndividualIDInSession() const
{
    return BestIndividualIDFromSession;
}

float UNsNNSessionSubsystem::GetBestPopulationIDInSession() const
{
    return BestGenerationIDFromSession;
}

uint8 UNsNNSessionSubsystem::GetSessionOverride() const
{
    return SessionOverrideFlags;
}

uint8& UNsNNSessionSubsystem::GetMutableSessionOverride()
{
    return SessionOverrideFlags;
}

FNsTrainSessionSetup UNsNNSessionSubsystem::GetSessionData() const
{
    return SessionData;
}

FNsTrainSessionSetup& UNsNNSessionSubsystem::GetMutableSessionData()
{
    return SessionData;
}

bool UNsNNSessionSubsystem::IsTraining() const
{
    return bIsTraining;
}
