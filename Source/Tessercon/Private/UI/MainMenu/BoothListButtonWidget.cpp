// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/BoothListButtonWidget.h"
#include "UI/MainMenu/BoothListWidget.h"

#include "Blueprint/AsyncTaskDownloadImage.h"

#include "UI/TesserconButton.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

UBoothListButtonWidget* UBoothListButtonWidget::p_SelectedButton = nullptr;

void UBoothListButtonWidget::NativeConstruct()
{
	if (BoothListButton != nullptr)
	{
		this->SetupBoothListButton();
	}
}

void UBoothListButtonWidget::NativeDestruct()
{
	//TODO
}

void UBoothListButtonWidget::SetInfo(UBoothListWidget* parent, const FString& boothid, const FString& boothname, const FString& boothlocation)
{
	p_Parent = parent;
	m_BoothID = boothid;
	BoothNameText->SetText(FText::FromString(boothname));
	BoothLocationText->SetText(FText::FromString(boothlocation));
}

void UBoothListButtonWidget::SetupBoothListButton()
{
	BoothListButton->OnClicked.AddDynamic(this, &UBoothListButtonWidget::OnButtonClicked);
}

void UBoothListButtonWidget::OnButtonClicked()
{
	if (BoothListButton->p_SelectedButton != nullptr)
	{
		BoothListButton->p_SelectedButton->SetNormal();
	}
	BoothListButton->SetSelected();
	BoothListButton->p_SelectedButton = BoothListButton;

	if (p_Parent != nullptr)
	{
		p_Parent->GetBoothInfo(m_BoothID);
	}
}

void UBoothListButtonWidget::GetBoothImage(UTexture2DDynamic* Texture)
{
	BoothImage->SetBrushSize(FVector2D(49.0f, 49.0f));
	BoothImage->SetBrushFromTextureDynamic(Texture);
}