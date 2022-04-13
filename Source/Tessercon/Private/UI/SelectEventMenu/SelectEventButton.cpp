// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectEventMenu/SelectEventButton.h"
#include "UI/SelectEventMenu/SelectEventMenuWidget.h"

#include "Components/Image.h"

USelectEventButton::USelectEventButton()
{
	FString Path = "/Game/BaseCore/Textures/SelectEvent/SelectedEventButton";
	NormalImage = LoadObject<UTexture2D>(NULL, *Path, NULL, LOAD_None, NULL);
	HoverImage = LoadObject<UTexture2D>(NULL, *Path, NULL, LOAD_None, NULL);
	SelectedImage = LoadObject<UTexture2D>(NULL, *Path, NULL, LOAD_None, NULL);
	if (NormalImage != nullptr && SelectedImage != nullptr)
	{
		NormalBrush.SetResourceObject(NormalImage);
		NormalBrush.DrawAs = ESlateBrushDrawType::Type::NoDrawType;
		HoverBrush.SetResourceObject(SelectedImage);
		SelectedBrush.SetResourceObject(SelectedImage);
		this->WidgetStyle.SetNormal(NormalBrush);
		this->WidgetStyle.SetPressed(NormalBrush);
		this->WidgetStyle.SetHovered(HoverBrush);
		this->WidgetStyle.SetNormalPadding(FMargin(8.0f, 8.0f));
	}
	this->OnClicked.AddDynamic(this, &USelectEventButton::OnButtonClicked);
}

void USelectEventButton::SetInfo(USelectEventMenuWidget* Parent, const FString& eventid)
{
	p_Parent = Parent;
	m_EventID = eventid;
}

void USelectEventButton::OnButtonClicked()
{
	if (p_SelectedButton != nullptr)
	{
		p_SelectedButton->SetNormal();
	}

	this->SetSelected();

	p_SelectedButton = this;

	if (p_Parent != nullptr)
	{
		p_Parent->GetEventInfo(m_EventID);
	}
}

void USelectEventButton::GetEventImage(UTexture2DDynamic* Texture)
{
	UImage* EventBanner = NewObject<UImage>();
	EventBanner->SetBrushSize(FVector2D(269.0f, 133.0f));
	EventBanner->SetBrushFromTextureDynamic(Texture);
	this->AddChild(EventBanner);
}
