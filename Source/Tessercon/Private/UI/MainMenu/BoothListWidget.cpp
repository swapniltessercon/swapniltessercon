// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/BoothListWidget.h"
#include "UI/MainMenu/BoothListButtonWidget.h"
#include "UI/MainMenu/TeamInfoProfileBarWidget.h"

#include "TesserconGameInstance.h"

#include "Blueprint/AsyncTaskDownloadImage.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "Engine/Font.h"

UBoothListWidget::UBoothListWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//TODO
	static ConstructorHelpers::FClassFinder<UUserWidget> BoothListButtonWidgetBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/BoothListButton_WBP"));
	if (BoothListButtonWidgetBPObj.Class != nullptr)
	{
		BoothListButtonWidgetClass = BoothListButtonWidgetBPObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> ExhibitorListButtonWidgetBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/TeamListProfileBar_wBP"));
	if (ExhibitorListButtonWidgetBPObj.Class != nullptr)
	{
		ExhibitorListButtonWidgetClass = ExhibitorListButtonWidgetBPObj.Class;
	}
}

void UBoothListWidget::NativeConstruct()
{
	//TODO
	if (BoothListCloseButton != nullptr)
	{
		BoothListCloseButton->OnClicked.AddDynamic(this, &UBoothListWidget::RemoveFromParent);
	}

	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			UTesserconGameInstance* GameInstance = static_cast<UTesserconGameInstance*>(GetGameInstance());
			
			TessClient->Getsession().GetBoothListForEvent(GameInstance->GetEvent().EventID, m_BoothList);
		}
	}

	if (BoothListScrollBox != nullptr)
	{
		this->PopulateBoothList();
	}

	if (WidgetBackgroundImage != nullptr)
	{
		WidgetBackgroundImage->OnMouseButtonDownEvent.BindUFunction(this, "OnMouseButtonDownImage");
	}

	if (SearchEditableTextBox != nullptr)
	{
		SearchEditableTextBox->OnTextChanged.AddDynamic(this, &UBoothListWidget::OnSearchContentChanged);
	}
}

void UBoothListWidget::NativeDestruct()
{
	UBoothListButtonWidget::p_SelectedButton = nullptr;
}

void UBoothListWidget::PopulateBoothList()
{
	TCHAR Location = '-1';
	UVerticalBox* LocationVerticalBox = nullptr;
	UTextBlock* LocationText = nullptr;
	for (int i = 0; i < m_BoothList.Num(); i++)
	{
		if (m_BoothList[i].BoothLocation[0] != Location)
		{
			LocationVerticalBox = NewObject<UVerticalBox>();
			LocationText = NewObject<UTextBlock>();
			UObject* obj_ptr = StaticLoadObject(UFont::StaticClass(), NULL, TEXT("/Game/BaseCore/Fonts/Poppins-Regular_Font"));
			UFont* font_ptr = Cast<UFont>(obj_ptr);
			LocationText->SetFont(FSlateFontInfo(font_ptr, 11));
			LocationText->SetText(FText::FromString("  Section " + m_BoothList[i].BoothLocation.LeftChop(1)));
			LocationVerticalBox->AddChild(LocationText);
			Location = m_BoothList[i].BoothLocation[0];
			BoothListScrollBox->AddChild(LocationVerticalBox);
		}
		UBoothListButtonWidget* BoothListMember = CreateWidget<UBoothListButtonWidget>(GetWorld(), BoothListButtonWidgetClass);
		if (BoothListMember != nullptr)
		{
			UAsyncTaskDownloadImage* UrlImageTask = NewObject<UAsyncTaskDownloadImage>();
			UrlImageTask->OnSuccess.AddDynamic(BoothListMember, &UBoothListButtonWidget::GetBoothImage);
			UrlImageTask->Start(m_BoothList[i].BoothBannerURL);
			BoothListMember->SetInfo(this, m_BoothList[i].BoothID, m_BoothList[i].BoothName, m_BoothList[i].BoothLocation);
			LocationVerticalBox->AddChild(BoothListMember);
			if (i == 0)
			{
				BoothListMember->OnButtonClicked();
			}
		}
	}
}

FEventReply UBoothListWidget::OnMouseButtonDownImage(FGeometry MyGeometry, const FPointerEvent& MousEvent)
{
	this->RemoveFromParent();
	return FEventReply(false);
}

void UBoothListWidget::GetBoothInfo(const FString& boothid)
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		if (TessClient != nullptr)
		{
			TesserconAPI::FOnBoothInfoForEventResponseDelegate BoothInfoDelegate;
			BoothInfoDelegate.BindUObject(this, &UBoothListWidget::BoothInfoCallBack);
			TessClient->Getsession().GetBoothInfoByBoothID(boothid, BoothInfoDelegate);
		}
	}
}

void UBoothListWidget::GetBoothInfoImage(UTexture2DDynamic* Texture)
{
	BoothLogoImage->SetBrushFromTextureDynamic(Texture);
	SelectedBoothBanner->SetBrushFromTextureDynamic(Texture);
}

void UBoothListWidget::OnSearchContentChanged(const FText& Text)
{
	FString SearchText = Text.ToLower().ToString();
	for (auto Parent : BoothListScrollBox->GetAllChildren())
	{
		UScrollBox* ParentWidget = static_cast<UScrollBox*>(Parent);
		for (auto ChildWidget : ParentWidget->GetAllChildren())
		{
			UBoothListButtonWidget* BoothListWidget = Cast<UBoothListButtonWidget>(ChildWidget);
			if (!SearchText.IsEmpty())
			{
				if (BoothListWidget != nullptr)
				{
					FString NameOfWidget = BoothListWidget->BoothNameText->GetText().ToString();
					if (NameOfWidget.Contains(SearchText))
					{
						BoothListWidget->SetVisibility(ESlateVisibility::Visible);
						ParentWidget->GetAllChildren()[0]->SetVisibility(ESlateVisibility::Visible);
					}
					else
					{
						BoothListWidget->SetVisibility(ESlateVisibility::Collapsed);
					}
				}
				else
				{
					ChildWidget->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
			else
			{
				ChildWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}		
	}
}

void UBoothListWidget::BoothInfoCallBack(const ETesserconError& Error, const FTesserconBooth& BoothInfo)
{
	UAsyncTaskDownloadImage* UrlImageTask = NewObject<UAsyncTaskDownloadImage>();
	UrlImageTask->OnSuccess.AddDynamic(this, &UBoothListWidget::GetBoothInfoImage);
	UrlImageTask->Start(BoothInfo.BoothBannerURL);

	BoothNameText->SetText(FText::FromString(BoothInfo.BoothName));
	BoothLocationText->SetText(FText::FromString(BoothInfo.BoothLocation));
	BoothDescriptionText->SetText(FText::FromString(BoothInfo.BoothDescription));
	CompanyEmailText->SetText(FText::FromString(BoothInfo.CompanyEmail));
	CompanyPhoneText->SetText(FText::FromString(BoothInfo.CompanyPhone));
	this->PopulateExhibitorList(BoothInfo.BoothExhibitors);
}

void UBoothListWidget::PopulateExhibitorList(const TArray<FTesserconUserInfo>& ExhibitorList)
{
	if (ExhibitorListScrollBox != nullptr)
	{
		ExhibitorListScrollBox->ClearChildren();

		if (ExhibitorListButtonWidgetClass == nullptr) return;

		for (auto Exhibitor : ExhibitorList)
		{
			UTeamInfoProfileBarWidget* ExhibitorListMember = CreateWidget<UTeamInfoProfileBarWidget>(GetWorld(), ExhibitorListButtonWidgetClass);
			if (ExhibitorListMember == nullptr) return;
			ExhibitorListMember->SetProfile(nullptr, Exhibitor);
			ExhibitorListMember->UserTypeTextBolock->SetText(FText::FromString("E"));
			ExhibitorListScrollBox->AddChild(ExhibitorListMember);
		}
	}
}
