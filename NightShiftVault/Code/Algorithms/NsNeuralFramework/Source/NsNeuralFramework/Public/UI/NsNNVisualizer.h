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
    virtual void NativeConstruct() override;
    virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual bool NativeIsInteractable() const override;
    virtual bool NativeSupportsKeyboardFocus() const override;
    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    //~ End UUserWiget Interface

// Variables
public:

    /** Neural Network */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TObjectPtr<class UNsNNArchitecture> NeuralNetwork;

    /** Pan Offset */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    FVector2D PanOffset;

    /** Pan Offset */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    bool bIsPanning;

    /** Zoom factor */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Visualization")
    float ZoomFactor;
};
