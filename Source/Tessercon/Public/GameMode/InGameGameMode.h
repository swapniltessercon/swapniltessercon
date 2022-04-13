// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "InGameGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API AInGameGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AInGameGameMode();

protected:

	virtual void BeginPlay() override;
	void PostLogin(APlayerController* NewPlayer) override;
	//virtual void Logout(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	//void Logout(APlayerController* NewPlayer) override;
};
