// Copyright (C) 2024 mykaa. All rights reserved.

#include "UI/NsNNVisualizer.h"
#include "Networks/NsNNArchitecture.h"

int32 UNsNNVisualizer::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

#if WITH_EDITOR
    if (NeuralNetwork == nullptr)
    {
        return LayerId;
    }

    const TArray<FVector2D> NodePositions = NeuralNetwork->GetNodePositions();
    const TArray<TTuple<int32, int32, float>> Connections = NeuralNetwork->GetConnections();
    const FVector2D Offset = AllottedGeometry.GetAbsolutePosition();

    const FVector2D NodeSize(20.f, 20.f);
    const FSlateBrush* const DefaultBrush = FCoreStyle::Get().GetBrush(TEXT("WhiteBrush"));
    for (const FVector2D& Position : NodePositions)
    {
        FSlateDrawElement::MakeBox(OutDrawElements
                                 , LayerId
                                 , AllottedGeometry.ToPaintGeometry(FVector2f(NodeSize)
                                 , FSlateLayoutTransform(Position + Offset - NodeSize * 0.5f))
                                 , DefaultBrush
                                 , ESlateDrawEffect::None
                                 , FLinearColor::Gray);
    }

    for (const auto& Connection : Connections)
    {
        const FVector2D StartPos = NodePositions[Connection.Get<0>()] + Offset, EndPos = NodePositions[Connection.Get<1>()] + Offset;
        TArray<FVector2D> LinePoints = { StartPos, EndPos };
        FSlateDrawElement::MakeLines(OutDrawElements
                                   , LayerId
                                   , AllottedGeometry.ToPaintGeometry()
                                   , LinePoints
                                   , ESlateDrawEffect::None
                                   , Connection.Get<2>() > 0.f ? FColor::Green : FColor::Red)
                                   , true
                                   , FMath::Abs(Connection.Get<2>() * 1.25f);
    }
#endif // WITH_EDITOR

    return LayerId + 1;
}