// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

/**
 * 
 */

UENUM()
enum class EInputModeType : uint8
{
	UIOnly,
	GameAndUI,
	GameOnly
};

UCLASS()
class TESSERCON_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void Setup(EInputModeType InputType);
	
	virtual void Teardown();
};
