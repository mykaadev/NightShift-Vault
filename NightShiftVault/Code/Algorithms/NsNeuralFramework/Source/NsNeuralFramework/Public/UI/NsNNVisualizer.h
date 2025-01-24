// Copyright (C) 2024 mykaa. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NsNNVisualizer.generated.h"

/**
 * Network Visualizer Widget
 */
UCLASS(BlueprintType, Blueprintable)
class NSNEURALFRAMEWORK_API UNsNNVisualizer : public UUserWidget
{
    GENERATED_BODY()

// Functions
protected:

    //~ Begin UUserWiget Interface
    virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    //~ End UUserWiget Interface

// Variables
public:

    /** Neural Network */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TObjectPtr<class UNsNNArchitecture> NeuralNetwork;
};
