// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenu/MapMenuWidget.h"
#include "UI/InGameMenu/MapBoothButton.h"

#include "TesserconGameInstance.h"

#include "Blueprint/AsyncTaskDownloadImage.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "Engine/Font.h"

UMapMenuWidget::UMapMenuWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MapBoothButtonWidgetBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/InGameMenu/MapBoothButton_WBP"));
	if (MapBoothButtonWidgetBPObj.Class != nullptr)
	{
		MapBoothButtonWidgetClass = MapBoothButtonWidgetBPObj.Class;
	}
}

void UMapMenuWidget::NativeConstruct()
{
	//TODO
	if (MapMenuCloseButton != nullptr)
	{
		MapMenuCloseButton->OnClicked.AddDynamic(this, &UMapMenuWidget::RemoveFromParent);
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
}

void UMapMenuWidget::NativeDestruct()
{
	//TODO
}

void UMapMenuWidget::SetupMap(float InZoom, float InDimension, UTexture2D* InImage)
{
	m_Zoom = InZoom;
	m_Dimension = InDimension;
	if (MapImage != nullptr)
	{
		MapImage->SetBrushFromTexture(InImage);
	}

	if (SearchEditableTextBox != nullptr)
	{
		SearchEditableTextBox->OnTextChanged.AddDynamic(this, &UMapMenuWidget::OnSearchContentChanged);
	}
}

void UMapMenuWidget::PopulateBoothList()
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
		UMapBoothButton* BoothListMember = CreateWidget<UMapBoothButton>(GetWorld(), MapBoothButtonWidgetClass);
		if (BoothListMember != nullptr)
		{
			UAsyncTaskDownloadImage* UrlImageTask = NewObject<UAsyncTaskDownloadImage>();
			UrlImageTask->OnSuccess.AddDynamic(BoothListMember, &UMapBoothButton::GetBoothImage);
			UrlImageTask->Start(m_BoothList[i].BoothBannerURL);
			BoothListMember->SetInfo(this, m_BoothList[i].BoothID, m_BoothList[i].BoothName, m_BoothList[i].BoothLocation, m_BoothList[i].BoothMapLocation);
			LocationVerticalBox->AddChild(BoothListMember);
		}
	}
}

void UMapMenuWidget::OnSearchContentChanged(const FText& Text)
{
	FString SearchText = Text.ToLower().ToString();
	for (auto Parent : BoothListScrollBox->GetAllChildren())
	{
		UScrollBox* ParentWidget = static_cast<UScrollBox*>(Parent);
		for (auto ChildWidget : ParentWidget->GetAllChildren())
		{
			UMapBoothButton* BoothListWidget = Cast<UMapBoothButton>(ChildWidget);
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
