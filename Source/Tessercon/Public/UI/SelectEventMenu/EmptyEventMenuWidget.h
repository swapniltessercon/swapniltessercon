// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/MenuWidget.h"

#include "TesserconCore.h"

#include "EmptyEventMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UEmptyEventMenuWidget : public UMenuWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:
	UPROPERTY(meta = (BindWidget))
		class UButton* LogoutButton;

	UFUNCTION()
		void OnLogoutButtonClicked();

	
};
