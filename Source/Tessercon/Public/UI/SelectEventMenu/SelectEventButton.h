// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/TesserconButton.h"
#include "SelectEventButton.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API USelectEventButton : public UTesserconButton
{
	GENERATED_BODY()
public:
	USelectEventButton();
private:
	UFUNCTION()
		void GetEventImage(UTexture2DDynamic* Texture);

	UFUNCTION()
		void OnButtonClicked();
	
	void SetInfo(class USelectEventMenuWidget* Parent, const FString& eventid);

	FString m_EventID;

	class USelectEventMenuWidget* p_Parent;

	friend class USelectEventMenuWidget;
};
