// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Character/InGamecharacter.h"

#include "TesserconPlayerInfoComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESSERCON_API UTesserconPlayerInfoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTesserconPlayerInfoComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Replacting
	//ReplicatedUsing does the same thing, but enables us to set a RepNotify function that will be triggered when a client successfully receives the replicated data.
	//We will use OnRep_PlayerInfo to perform updates to each client based on changes to this variable
	UPROPERTY(ReplicatedUsing = OnRep_PlayerInfo)
		FPlayerInfo m_PlayerInfo;

	UFUNCTION(BlueprintCallable)
		void InitializePlayerInfo();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//UFUNCTION(Server)
	//To declare a function as an RPC that will be called on the client, but executed on the server is very similar but uses the Server keyword
	UFUNCTION(Server, Reliable)
		void Server_SendPlayerInfo(FPlayerInfo InPlayerInfo);

	UFUNCTION()
		void OnRep_PlayerInfo();

	void UpdatePlayerInfo(const FPlayerInfo& InPlayerInfo);
	void AutonmousProxy_OnRep_PlayerInfo();
	void SimulatedProxy_OnRep_PlayerInfo();
	bool IsPlayerInfoEqual(const FPlayerInfo& InPlayerInfo);
	void SetCharacterNamePlateAndMesh();
	void AttemptUpdatePlayerInfo(const FPlayerInfo& InPlayerInfo);
};
