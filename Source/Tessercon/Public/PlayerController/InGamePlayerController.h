// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InGamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API AInGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AInGamePlayerController();

	UFUNCTION(reliable, client)
		void ClientJoinVoice(const FString& GameMode, const FString& OnlineSessionId, const int32& TeamNum = -1);


protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;

private:
	virtual void SetupInputComponent() override;

	
};
