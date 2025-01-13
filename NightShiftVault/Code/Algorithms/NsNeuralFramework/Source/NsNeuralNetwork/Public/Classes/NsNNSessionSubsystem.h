// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Libraries/NsNNTypeLibrary.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NsNNSessionSubsystem.generated.h"

/**
 * Subsystem mode used to manage the training sessions
 */
UCLASS()
class NSNEURALFRAMEWORK_API UNsNNSessionSubsystem : public UGameInstanceSubsystem
{
  GENERATED_BODY()

// Functions
public:

    /** Constructor */
    UNsNNSessionSubsystem();

    /** Get Session Subsystem */
    UFUNCTION(BlueprintCallable, Category = "Neural Framework")
    static UNsNNSessionSubsystem* GetSubsystem();

    /** Get Control Panel */
    UFUNCTION()
    class UNsNNControlPanel* GetControlPanel() const;

    /** Initialize session */
    UFUNCTION()
    void Init();

    /** Set Random Seed */
    UFUNCTION()
    void SetRandomSeed(const int32 InRandomSeed);

    /** Set Population Data */
    UFUNCTION()
    void SetPopulationParameters(const int32 InPopulationSize, const int32 InMaxTimePerIndividual);

    /** Set Mutation and Recombination Data */
    UFUNCTION()
    void SetMutationAndRecombinationParameters(const float InMutationChance, const float InRecombinationChance);

    /** Set the Neural Network Structure */
    UFUNCTION()
    void SetNeuralNetworkStructure(const int32 InNeuralInputs, const int32 InNeuralHiddenLayerSize, const int32 InNeuralOutputs);

    /** Set the Session Data */
    UFUNCTION(BlueprintCallable)
    void SetSessionData(const FNsTrainSessionSetup& InSessionData);

    /** Get Inputs */
    UFUNCTION()
    int32 GetInputSize() const;

    /** Get Neural Network */
    UFUNCTION()
    class UNsNNBaseNetwork* GetNeuralNetwork() const;

    /** Set Neural Network */
    UFUNCTION()
    void SetNeuralNetwork(class UNsNNBaseNetwork* const InNetwork);

    /** Get Agent Controller */
    UFUNCTION()
    class ANsNNTrainController* GetAgentController() const;

    /** Set Agent Controller */
    UFUNCTION()
    void SetAgentController(class ANsNNTrainController* const InController);

    /** On Start Request Received */
    UFUNCTION()
    void OnStartRequestReceived();

    /** Initialize Population */
    UFUNCTION()
    void InitializePopulation(int32 InIndividualSize);

    /** Set default session values */
    UFUNCTION()
    void SetDefaultSessionValues();

    /** Starts the train session */
    UFUNCTION()
    void StartTrainSession();

    /** On Hearth Beat */
    UFUNCTION()
    void OnHeartBeat();

    /** End Current Evaluation */
    UFUNCTION()
    void EndCurrentEvaluation();

    /** Compute Fitness */
    UFUNCTION()
    float ComputeFitness() const;

    /** Next Generation */
    UFUNCTION()
    void NextGeneration();

    /** Natural Selection */
    UFUNCTION()
    void NaturalSelection();

    /** Recombine */
    UFUNCTION()
    void Recombine();

    /** Mutate */
    UFUNCTION()
    void Mutate();

    /** Stop the train session */
    UFUNCTION()
    void StopTrainSession();

    /** Save the best genotype from session */
    UFUNCTION(BlueprintCallable)
    void SaveBestGenotypeFromSession();

    /** Save the current genotype from session */
    UFUNCTION(BlueprintCallable)
    void SaveCurrentGenotype();

    /** Save session data */
    UFUNCTION(BlueprintCallable)
    void SaveSessionData(const TArray<float>& InGenotype, const FString& InFileNameSuffix);

    /** Load session data */
    UFUNCTION(BlueprintCallable)
    bool LoadSessionDataFromFile();

    /** Get Mutation chance */
    UFUNCTION(BlueprintCallable)
    float GetMutationChance() const;

    /** Get Max Time Per Individual */
    UFUNCTION(BlueprintCallable)
    int32 GetMaxTimePerIndividual() const;

    /** Get Current Individual Time */
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentTimePerIndividual() const;

    /** Get Current Individual ID */
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentIndividualID() const;

    /** Get Current Population ID */
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentPopulationID() const;

    /** Get Current Population ID */
    UFUNCTION(BlueprintCallable)
    int32 GetCurrentPopulationSize() const;

    /** Get Current Individual Fitness */
    UFUNCTION(BlueprintCallable)
    float GetCurrentIndividualFitness() const;

    /** Get Best Fitness In Session */
    UFUNCTION(BlueprintCallable)
    float GetBestFitnessInSession() const;

    /** Get Best Fitness In Population */
    UFUNCTION(BlueprintCallable)
    float GetBestFitnessInPopulation() const;

    /** Get Best Individual ID in Session */
    UFUNCTION(BlueprintCallable)
    int32 GetBestIndividualIDInSession() const;

    /** Get Best Population ID in Session */
    UFUNCTION(BlueprintCallable)
    float GetBestPopulationIDInSession() const;

    /** Get session overrides */
    UFUNCTION(BlueprintCallable)
    uint8 GetSessionOverride() const;

    /** Get mutable session overrides */
    UFUNCTION(BlueprintCallable)
    uint8& GetMutableSessionOverride();

    /** Get mutable session data */
    UFUNCTION(BlueprintCallable)
    FNsTrainSessionSetup GetSessionData() const;

    /** Get mutable session data */
    UFUNCTION(BlueprintCallable)
    FNsTrainSessionSetup& GetMutableSessionData();

// Variables
private:

    /** Control Panel */
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<class UNsNNControlPanel> ControlPanel;

    /** Session Data */
    UPROPERTY(VisibleAnywhere)
    FNsTrainSessionSetup SessionData;

    /** Random seed to be used during the train session */
    UPROPERTY(VisibleAnywhere)
    int32 RandomSeed;

    /** Random Stream */
    UPROPERTY()
    FRandomStream RandomStream;

    /** Agent Controller */
    UPROPERTY()
    TObjectPtr<class ANsNNTrainController> CurrentController;

    /** Neural Network */
    UPROPERTY()
    TObjectPtr<class UNsNNBaseNetwork> NeuralNetwork;

    /** Population */
    UPROPERTY()
    TArray<class UNsNNIndividual*> CurrentPopulation;

    /** Next Population */
    UPROPERTY()
    TArray<class UNsNNIndividual*> NextPopulation;

    /** Generation */
    UPROPERTY()
    int32 Generation;

    /** Best Individual */
    UPROPERTY()
    TObjectPtr<class UNsNNIndividual> BestIndividual;

    /** Current Individual Index */
    UPROPERTY()
    int32 CurrentIndividualIndex;

    /** Average Fitness Value */
    UPROPERTY()
    float AverageFitness;

    /** Fitness Sum */
    UPROPERTY()
    float FitnessSum;

    /** Best Fitness In Generation */
    UPROPERTY()
    float BestFitnessInGeneration;

    /** Best Fitness In Session */
    UPROPERTY()
    float BestFitnessInSession;

    /** Best Fitness Run */
    UPROPERTY()
    float BestFitnessRun;

    /** Best Individual ID In Session */
    UPROPERTY()
    int32 BestIndividualIDFromSession;

    /** Best Generation ID In Session */
    UPROPERTY()
    int32 BestGenerationIDFromSession;

    /** Individual Time */
    UPROPERTY()
    int32 IndividualTime;

    /** Current Individual Finished */
    UPROPERTY()
    bool bCurrentIndividualFinished;

    /** Is Currently Training */
    UPROPERTY()
    bool bIsTraining;

    /** Heart beat timer handle */
    UPROPERTY()
    FTimerHandle HeartBeatTimerHandle;

    /** Best Genotype from session */
    UPROPERTY()
    TArray<float> BestGenotypeFromSession;

    /** Session Override */
    UPROPERTY()
    uint8 SessionOverrideFlags;

    /** Genotype Override */
    UPROPERTY()
    TArray<float> SessionOverrideGenotype;
};
