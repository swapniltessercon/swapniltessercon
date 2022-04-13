// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectEventMenu/SelectEventMenuWidget.h"
#include "UI/SelectEventMenu/SelectEventButton.h"
#include "UI/Settings/SettingsMenuWidget.h"

#include "TesserconGameInstance.h"
#include "UI/HUD/InitialHUD.h"

#include "Blueprint/WidgetTree.h"
#include "Blueprint/AsyncTaskDownloadImage.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Spacer.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

USelectEventMenuWidget::USelectEventMenuWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> SettingsWidgetBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/Settings_WBP"));
	if (SettingsWidgetBPObj.Class != nullptr)
	{
		SettingsWidgetClass = SettingsWidgetBPObj.Class;
	}
}

void USelectEventMenuWidget::NativeConstruct()
{
	if (EventSelectButton != nullptr)
	{
		EventSelectButton->OnClicked.AddDynamic(this, &USelectEventMenuWidget::OnSelectEventButtonClicked);
	}

	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			m_EventList = TessClient->Getsession().GetEventList();
		}
	}

	if (EventListScrollBox != nullptr)
	{
		USelectEventButton*  EventListMember = NewObject<USelectEventButton>();

		EventListMember->SetInfo(this, m_EventList[0].EventID);

		UAsyncTaskDownloadImage* UrlImageTask = NewObject<UAsyncTaskDownloadImage>();
		UrlImageTask->OnSuccess.AddDynamic(EventListMember, &USelectEventButton::GetEventImage);
		UrlImageTask->Start(m_EventList[0].EventBannerURL);

		EventListMember->OnButtonClicked();
		EventListScrollBox->AddChild(EventListMember);

		for (int i = 1; i < m_EventList.Num(); i++)
		{
			EventListMember = NewObject<USelectEventButton>();

			UAsyncTaskDownloadImage* ImageTask = NewObject<UAsyncTaskDownloadImage>();
			ImageTask->OnSuccess.AddDynamic(EventListMember, &USelectEventButton::GetEventImage);
			ImageTask->Start(m_EventList[i].EventBannerURL);
			
			EventListMember->SetInfo(this, m_EventList[i].EventID);
			EventListScrollBox->AddChild(EventListMember);
		}

		if (m_EventList.Num() == 1)
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController != nullptr)
			{
				AInitialHUD* HUD = Cast<AInitialHUD>(PlayerController->GetHUD());
				if (HUD != nullptr)
				{
					this->OnSelectEventButtonClicked();
				}
			}
		}
	}

	if (VisitOrganiserButton != nullptr)
	{
		VisitOrganiserButton->OnClicked.AddDynamic(this, &USelectEventMenuWidget::OpenOrganiserContactUrl);
	}

	if (SettingsButton != nullptr)
	{
		SettingsButton->OnClicked.AddDynamic(this, &USelectEventMenuWidget::OnSettingsButtonClicked);
	}
}

void USelectEventMenuWidget::NativeDestruct()
{
	USelectEventButton::p_SelectedButton = nullptr;
}

void USelectEventMenuWidget::GetEventInfo(const FString& EventID)
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TesserconAPI::FOnEventInfoForUserResponseDelegate EventInfoDelegate;
			EventInfoDelegate.BindUObject(this, &USelectEventMenuWidget::EventInfoCallBack);
			TessClient->Getsession().GetEventInfoByEventID(EventID, EventInfoDelegate);

			TesserconAPI::FOnEventOrganiserResponseDelegate EventOrganiserDelegate;
			EventOrganiserDelegate.BindUObject(this, &USelectEventMenuWidget::EventOrganiserCallBack);
			TessClient->Getsession().GetEventOrganiserByEventID(EventID, EventOrganiserDelegate);

			TesserconAPI::FOnEventSponsorsResponseDelegate EventSponsorsDelegate;
			EventSponsorsDelegate.BindUObject(this, &USelectEventMenuWidget::EventSponsorsCallBack);
			TessClient->Getsession().GetEventSponsorsByEventID(EventID, EventSponsorsDelegate);
		}
	}
}

void USelectEventMenuWidget::EventInfoCallBack(const ETesserconError& Error, const FTesserconEvent& EventInfo)
{
	EventInfoSizeBox->SetVisibility(ESlateVisibility::Hidden);
	UAsyncTaskDownloadImage* UrlImageTask = NewObject<UAsyncTaskDownloadImage>();
	UrlImageTask->OnSuccess.AddDynamic(this, &USelectEventMenuWidget::GetEventInfoImage);
	UrlImageTask->Start(EventInfo.EventBannerURL);

	StartTimeText->SetText(FText::FromString(EventInfo.StartTime.ToString()));
	EndTimeText->SetText(FText::FromString(EventInfo.EndTime.ToString()));
	EventDescriptionText->SetText(FText::FromString(EventInfo.Description));

	UTesserconGameInstance* GameInstance =  static_cast<UTesserconGameInstance*>(GetGameInstance());
	GameInstance->SetEvent(EventInfo);
	EventInfoSizeBox->SetVisibility(ESlateVisibility::Visible);
}

void USelectEventMenuWidget::EventOrganiserCallBack(const ETesserconError& Error, const FTesserconEventOrganiser& EventOrganiser)
{
	UAsyncTaskDownloadImage* UrlImageTask = NewObject<UAsyncTaskDownloadImage>();
	UrlImageTask->OnSuccess.AddDynamic(this, &USelectEventMenuWidget::GetEventOrganiserImage);
	UrlImageTask->Start(EventOrganiser.OrganiserLogoUrl);
	EventOrganiserText->SetText(FText::FromString(EventOrganiser.OrganiserName));
	m_OrganiserVisitUrl = EventOrganiser.OrganiserContactUrl;
}

void USelectEventMenuWidget::EventSponsorsCallBack(const ETesserconError& Error, const TArray<FString>& EventSponsorsUrl)
{
	EventSponsorsScrollBox->ClearChildren();
	UAsyncTaskDownloadImage* UrlImageTask = NewObject<UAsyncTaskDownloadImage>();
	UrlImageTask->OnSuccess.AddDynamic(this, &USelectEventMenuWidget::GetEventSponsorImage);
	for(int i = 0; i < EventSponsorsUrl.Num(); i++)
	{
		UrlImageTask->Start(EventSponsorsUrl[i]);
	}
}

void USelectEventMenuWidget::GetEventInfoImage(UTexture2DDynamic* Texture)
{
	UImage* EventBanner = NewObject<UImage>();
	EventBanner->SetBrushSize(FVector2D(279.0f, 143.0f));
	EventBanner->SetBrushFromTextureDynamic(Texture);

	EventBannerSizeBox->RemoveChildAt(0);

	EventBannerSizeBox->AddChild(EventBanner);
}

void USelectEventMenuWidget::GetEventOrganiserImage(UTexture2DDynamic* Texture)
{
	UImage* OrganiserLogo = NewObject<UImage>();
	OrganiserLogo->SetBrushSize(FVector2D(57.0f, 57.0f));
	OrganiserLogo->SetBrushFromTextureDynamic(Texture);

	EventOrganiserSizeBox->RemoveChildAt(0);

	EventOrganiserSizeBox->AddChild(OrganiserLogo);
}

void USelectEventMenuWidget::GetEventSponsorImage(UTexture2DDynamic* Texture)
{
	UImage* SponsorLogo = NewObject<UImage>();
	SponsorLogo->SetBrushFromTextureDynamic(Texture);
	SponsorLogo->SetBrushSize(FVector2D(150.0f, 150.0f));

	EventSponsorsScrollBox->AddChild(SponsorLogo);
	
	USpacer* Spacer = NewObject<USpacer>();
	Spacer->SetSize(FVector2D(80.0f, 20.0f));
	EventSponsorsScrollBox->AddChild(Spacer);
}

void USelectEventMenuWidget::OpenOrganiserContactUrl()
{
	FPlatformProcess::LaunchURL(*m_OrganiserVisitUrl, NULL, NULL);
}

void USelectEventMenuWidget::OnSelectEventButtonClicked()
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TessClient->Getsession().EventLogin();

			UWorld* World = GetWorld();
			if (!ensure(World != nullptr)) return;

			APlayerController* PlayerController = World->GetFirstPlayerController();
			if (!ensure(PlayerController != nullptr)) return;

			FString TravelURL = "/Game/BaseCore/Maps/MainMenu/Maps/MainMenuLobby";

			PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
		}		
	}
}

void USelectEventMenuWidget::OnSettingsButtonClicked()
{
	USettingsMenuWidget* SettingsMenuWidget = CreateWidget<USettingsMenuWidget>(GetWorld(), SettingsWidgetClass);
	if (SettingsMenuWidget != nullptr)
	{
		SettingsMenuWidget->AddToViewport();
	}
}
