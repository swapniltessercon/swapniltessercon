// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TesserconButton.h"

UTesserconButton* UTesserconButton::p_SelectedButton = nullptr;

void UTesserconButton::SetNormal()
{
	if (NormalImage != nullptr && HoverImage != nullptr && SelectedImage != nullptr)
	{
		NormalBrush.SetResourceObject(NormalImage);
		HoverBrush.SetResourceObject(HoverImage);
		SelectedBrush.SetResourceObject(SelectedImage);
		NormalBrush.TintColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
		this->WidgetStyle.SetNormal(NormalBrush);
		this->WidgetStyle.SetHovered(HoverBrush);
		this->WidgetStyle.SetPressed(NormalBrush);
	}
}

void UTesserconButton::SetSelected()
{
	if (NormalImage != nullptr && HoverImage != nullptr && SelectedImage != nullptr)
	{
		NormalBrush.SetResourceObject(NormalImage);
		HoverBrush.SetResourceObject(HoverImage);
		SelectedBrush.SetResourceObject(SelectedImage);
		this->WidgetStyle.SetNormal(SelectedBrush);
		this->WidgetStyle.SetHovered(SelectedBrush);
		this->WidgetStyle.SetPressed(SelectedBrush);
	}
}
