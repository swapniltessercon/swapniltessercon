// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/TesserconPlayerInfoComponent.h"

#include "Net/UnrealNetwork.h"
#include "TesserconGameInstance.h"
#include "DrawDebugHelpers.h"


FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}

// Sets default values for this component's properties
UTesserconPlayerInfoComponent::UTesserconPlayerInfoComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	SetIsReplicatedByDefault(true);
	m_PlayerInfo = FPlayerInfo();
}


void UTesserconPlayerInfoComponent::InitializePlayerInfo()
{
	AInGamecharacter* Character = Cast<AInGamecharacter>(GetOwner());
	if (Character != nullptr && Character->IsLocallyControlled())
	{
		UTesserconGameInstance* GameInstance = Cast<UTesserconGameInstance>(GetWorld()->GetGameInstance());
		if (GameInstance == nullptr) return;

		FTesserconUserInfo UserInfo;
		if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
		{
			TesserconSession* TessSession = &FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient()->Getsession();
			UserInfo = TessSession->GetLoggedInUserInfo();
		}

		FPlayerInfo NewPlayerInfo = FPlayerInfo(UserInfo.FirstName + UserInfo.LastName, UserInfo.Username, UserInfo.Designation, UserInfo.UserRole);
		AttemptUpdatePlayerInfo(NewPlayerInfo);
	}
}

// Called when the game starts
void UTesserconPlayerInfoComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	InitializePlayerInfo();
}


// Called every frame
void UTesserconPlayerInfoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTesserconPlayerInfoComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTesserconPlayerInfoComponent, m_PlayerInfo);
}

void UTesserconPlayerInfoComponent::Server_SendPlayerInfo_Implementation(FPlayerInfo InPlayerInfo)
{
	UpdatePlayerInfo(InPlayerInfo);
}

void UTesserconPlayerInfoComponent::OnRep_PlayerInfo()
{
	switch (GetOwnerRole())
	{
	case ROLE_AutonomousProxy:
		AutonmousProxy_OnRep_PlayerInfo();
		break;
	case ROLE_SimulatedProxy:
		SimulatedProxy_OnRep_PlayerInfo();
		break;
	default:
		break;
	}
}

void UTesserconPlayerInfoComponent::UpdatePlayerInfo(const FPlayerInfo& InPlayerInfo)
{
	if (IsPlayerInfoEqual(InPlayerInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("in UpdatePlayerInfo ---- Player info equal "));
	}

	m_PlayerInfo = InPlayerInfo;

	if (IsRunningDedicatedServer()) return;
	SetCharacterNamePlateAndMesh();
}

void UTesserconPlayerInfoComponent::AutonmousProxy_OnRep_PlayerInfo()
{
	UE_LOG(LogTemp, Warning, TEXT("in AutonmousProxy_OnRep_PlayerInfo"));

	SetCharacterNamePlateAndMesh();
}

void UTesserconPlayerInfoComponent::SimulatedProxy_OnRep_PlayerInfo()
{
	UE_LOG(LogTemp, Warning, TEXT("in SimulatedProxy_OnRep_PlayerInfo"));

	SetCharacterNamePlateAndMesh();
}

bool UTesserconPlayerInfoComponent::IsPlayerInfoEqual(const FPlayerInfo& InPlayerInfo)
{
	if (m_PlayerInfo.MatrixUsername != InPlayerInfo.MatrixUsername) return false;
	if (m_PlayerInfo.Name != InPlayerInfo.Name) return false;
	if (m_PlayerInfo.UserDesignation != InPlayerInfo.UserDesignation) return false;

	return true;
}

void UTesserconPlayerInfoComponent::SetCharacterNamePlateAndMesh()
{
	AInGamecharacter* Character = Cast<AInGamecharacter>(GetOwner());
	if (Character == nullptr) return;

	Character->UpdateNamePlate(m_PlayerInfo.Name, m_PlayerInfo.MatrixUsername, m_PlayerInfo.UserDesignation, m_PlayerInfo.UserRole);
}

void UTesserconPlayerInfoComponent::AttemptUpdatePlayerInfo(const FPlayerInfo& InPlayerInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("in AttemptUpdatePlayerInfo"));

	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		UE_LOG(LogTemp, Warning, TEXT("in ROLE_AutonomousProxy"));
		Server_SendPlayerInfo(InPlayerInfo);
	}

	// We are the server and in control of the pawn.
	if (GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		UE_LOG(LogTemp, Warning, TEXT("In ROLE_SimulatedProxy"));
		UpdatePlayerInfo(InPlayerInfo);
	}

	//who has authority over a particular actor
	if (GetOwnerRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("In ROLE_Authority"));
		UpdatePlayerInfo(InPlayerInfo);
	}

	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		UE_LOG(LogTemp, Warning, TEXT("In ROLE_SimulatedProxy"));

		// ClientTick(DeltaTime); do nothing
	}
}
