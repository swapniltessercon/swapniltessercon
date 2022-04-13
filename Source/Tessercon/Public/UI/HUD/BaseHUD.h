// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "TesserconCore.h"
#include "UI/BaseMenuWidget.h"

#include "BaseHUD.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API ABaseHUD : public AHUD
{
	GENERATED_BODY()
public:
	class UBaseMenuWidget* Menu;
	TSubclassOf<UUserWidget> MenuClass;
	UFUNCTION(BlueprintCallable)
		class UBaseMenuWidget* GetCurrentMenuWidget() const { return Menu; }
protected:
	void BeginPlay() override;
	void OnLoginStateChanged(ETesserconLoginState LoginState);
};
