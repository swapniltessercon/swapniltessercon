// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectEventMenu/EmptyEventMenuWidget.h"

#include "Components/Button.h"

void UEmptyEventMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (LogoutButton)
	{
		LogoutButton->OnClicked.AddDynamic(this, &UEmptyEventMenuWidget::OnLogoutButtonClicked);
	}
}

void UEmptyEventMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UEmptyEventMenuWidget::OnLogoutButtonClicked()
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TessClient->Getsession().Logout();
		}
	}
}


