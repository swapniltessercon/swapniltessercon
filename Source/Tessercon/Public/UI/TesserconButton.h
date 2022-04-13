// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "TesserconButton.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UTesserconButton : public UButton
{
	GENERATED_BODY()
public:
	void SetNormal();

	void SetSelected();

	static UTesserconButton* p_SelectedButton;
protected:

	FSlateBrush NormalBrush;

	FSlateBrush HoverBrush;

	FSlateBrush SelectedBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* NormalImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* HoverImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UTexture2D* SelectedImage;
};
