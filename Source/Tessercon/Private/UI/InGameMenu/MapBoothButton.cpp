// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenu/MapBoothButton.h"
#include "Character/InGamecharacter.h"

#include "components/Button.h"
#include "components/Image.h"
#include "components/TextBlock.h"

void UMapBoothButton::NativeConstruct()
{
	if (MapBoothButton != nullptr)
	{
		this->SetupBoothListButton();
	}
}

void UMapBoothButton::NativeDestruct()
{
	//TODO
}

void UMapBoothButton::SetupBoothListButton()
{
	TeleportButton->OnClicked.AddDynamic(this, &UMapBoothButton::OnTeleportButtonClicked);
}

void UMapBoothButton::SetInfo(UUserWidget* InParent, const FString& InBoothID, const FString& InBoothName, const FString& InBoothLocation, const FVector& InBoothMapLocation)
{
	p_Parent = InParent;
	m_BoothID = InBoothID;
	BoothNameText->SetText(FText::FromString(InBoothName));
	BoothLocationText->SetText(FText::FromString(InBoothLocation));
	m_BoothMapLocation = InBoothMapLocation;
}

void UMapBoothButton::OnTeleportButtonClicked()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();

	AInGamecharacter* Character = Cast<AInGamecharacter>(playerController->GetCharacter());

	if (Character == nullptr) return;
	Character->AttemptTeleportInfo(m_BoothMapLocation);
	p_Parent->RemoveFromParent();
}

void UMapBoothButton::GetBoothImage(UTexture2DDynamic* Texture)
{
	BoothImage->SetBrushSize(FVector2D(49.0f, 49.0f));
	BoothImage->SetBrushFromTextureDynamic(Texture);
}