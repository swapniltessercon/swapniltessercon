// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/MenuWidget.h"
#include "TesserconCore.h"

#include "MapMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UMapMenuWidget : public UMenuWidget
{
	GENERATED_BODY()
public:
	UMapMenuWidget(const FObjectInitializer& ObjectInitializer);

	void SetupMap(float InZoom, float InDimension, UTexture2D* InImage);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_Zoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float m_Dimension;

protected:

	void NativeConstruct();

	void NativeDestruct();

private:

	TArray<FTesserconBoothList> m_BoothList;

	TSubclassOf<UUserWidget> MapBoothButtonWidgetClass;

	void PopulateBoothList();

	UPROPERTY(meta = (BindWidget))
		class UButton* MapMenuCloseButton;

	UPROPERTY(meta = (BindWidget))
		class UScrollBox* BoothListScrollBox;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* SearchEditableTextBox;

	UPROPERTY(meta = (BindWidget))
		class UImage* MapImage;

	UFUNCTION()
		void OnSearchContentChanged(const FText& Text);
};
