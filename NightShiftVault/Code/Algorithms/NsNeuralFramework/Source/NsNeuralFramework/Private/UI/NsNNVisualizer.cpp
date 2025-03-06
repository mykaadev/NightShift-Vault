// Copyright (C) 2024 mykaa. All rights reserved.

#include "UI/NsNNVisualizer.h"
#include "Networks/NsNNArchitecture.h"

void UNsNNVisualizer::NativeConstruct()
{
    Super::NativeConstruct();
    PanOffset = FVector2D::ZeroVector;
    bIsPanning = false;
    ZoomFactor = 1.0f;
    SetIsFocusable(true);
}

int32 UNsNNVisualizer::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FPaintContext PaintContext = FPaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

#if WITH_EDITOR
    if (NeuralNetwork == nullptr)
    {
        return LayerId;
    }

    const TArray<FVector2D> NodePositions = NeuralNetwork->GetNodePositions();
    const TArray<TTuple<int32, int32, float>> Connections = NeuralNetwork->GetConnections();
    const FVector2D Offset = AllottedGeometry.GetLocalSize() * 0.1f;

    const FVector2D NodeSize = FVector2D(20.f, 20.f) * ZoomFactor; // Scale the node size based on zoom

    // Draw connections
    for (const auto& Connection : Connections)
    {
        const FVector2D StartPos = NodePositions[Connection.Get<0>()] * ZoomFactor + Offset + PanOffset;
        const FVector2D EndPos = NodePositions[Connection.Get<1>()] * ZoomFactor + Offset + PanOffset;
        const TArray<FVector2D> LinePoints = { StartPos + FVector2D(NodeSize.X, 0.f), EndPos - FVector2D(NodeSize.X, 0.f) };
        FSlateDrawElement::MakeLines(PaintContext.OutDrawElements
                                   , PaintContext.MaxLayer
                                   , AllottedGeometry.ToPaintGeometry()
                                   , LinePoints
                                   , ESlateDrawEffect::None
                                   , Connection.Get<2>() > 0.f ? FColor::FromHex(TEXT("#75C15CFF")) : FColor::FromHex(TEXT("#CD7A75FF")))
                                   , true
                                   , FMath::Abs(Connection.Get<2>() * 1.f);
    }

    // Draw nodes
    // Draw nodes and their values with proper text alignment
    for (int32 i = 0; i < NodePositions.Num(); ++i)
    {
        const FVector2D Position = NodePositions[i] * ZoomFactor + Offset + PanOffset;
        const float NodeValue = NeuralNetwork->GetNodeValues()[i];
        const FString NodeText = FString::Printf(TEXT("%.3f"), NodeValue);

        // Center the text inside the scaled node
        const FVector2D TextSize = FVector2D(10.f, 10.f);
        const FVector2D TextPosition = Position - TextSize * 0.5f; // Adjust text position to be centered

        // Draw the node value text inside the circle
        FSlateDrawElement::MakeText(PaintContext.OutDrawElements
                                  , PaintContext.MaxLayer
                                  , AllottedGeometry.ToPaintGeometry(FSlateLayoutTransform(TextPosition))
                                  , NodeText
                                  , FCoreStyle::Get().GetFontStyle("NormalFont")
                                  , ESlateDrawEffect::None
                                  , FColor::White // Ensures contrast against the circle
        );
    }
#endif // WITH_EDITOR

    return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}


bool UNsNNVisualizer::NativeIsInteractable() const
{
    return true;
}

bool UNsNNVisualizer::NativeSupportsKeyboardFocus() const
{
    return true;
}

FReply UNsNNVisualizer::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsPanning)  // Only move if panning is active
    {
        PanOffset += InMouseEvent.GetCursorDelta();
        Invalidate(EInvalidateWidget::Paint); // Force a redraw
    }

    return FReply::Handled();
}

FReply UNsNNVisualizer::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        bIsPanning = false;
    }
    return FReply::Handled().ReleaseMouseCapture();
}

FReply UNsNNVisualizer::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        bIsPanning = true;
        return FReply::Handled().CaptureMouse(TakeWidget());
    }

    return FReply::Unhandled();
}

FReply UNsNNVisualizer::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    constexpr float ZoomStep = 0.1f;
    constexpr float MinZoom = 0.25f;
    constexpr float MaxZoom = 5.0f;

    ZoomFactor = FMath::Clamp(ZoomFactor + (InMouseEvent.GetWheelDelta() * ZoomStep), MinZoom, MaxZoom);

    Invalidate(EInvalidateWidget::Paint); // Force a redraw after zoom change
    return FReply::Handled();
}
