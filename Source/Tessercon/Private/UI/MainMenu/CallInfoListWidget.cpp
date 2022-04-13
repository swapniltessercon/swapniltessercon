// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/CallInfoListWidget.h"
//class
#include "UI/MainMenu/CallerNotifyProfileWidget.h"
//Components
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"


UCallInfoListWidget::UCallInfoListWidget(const FObjectInitializer& ObjectInitializer) 
	:Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> NotifyCallerBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/MainMenu/CallerNotificationBar_WBP"));
	if (NotifyCallerBPObj.Class != nullptr)
	{
		CallerNotifyProfileClass = NotifyCallerBPObj.Class;
	}
}
void UCallInfoListWidget::NativeConstruct()
{
	//TODO
	Super::NativeConstruct();
	if (CallBGImage != nullptr)
	{
		CallBGImage->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDownImage"));
	}
}

void UCallInfoListWidget::HideInforamtionBox()
{
	if (MicInfoSizeBox != nullptr)
	{ 
		MicInfoSizeBox->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCallInfoListWidget::ShowMicInforamtion()
{
	if (MicInfoSizeBox != nullptr)
	{
		MicInfoSizeBox->SetVisibility(ESlateVisibility::Visible);
	}
}



void UCallInfoListWidget::CallNotifinctionWindowSetup(EMatrixCallEventType EventType, FString RoomID, FMatrixCallEvent CalledInfo)
{
	UCallerNotifyProfileWidget* Widget = CreateWidget<UCallerNotifyProfileWidget>(GetWorld(), CallerNotifyProfileClass);
	if (Widget != nullptr && CallListScrollBox != nullptr)
	{
		CallListScrollBox->AddChild(Widget);
		Widget->InitalSetup(this, CalledInfo, RoomID);
	}
}

void UCallInfoListWidget::NativeDestruct()
{
	//TODO
	Super::NativeDestruct();
}

FEventReply UCallInfoListWidget::OnMouseButtonDownImage(FGeometry MyGeometry, const FPointerEvent& MousEvent)
{
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController != nullptr)
	{
		ABaseHUD* HUD = Cast<ABaseHUD>(PlayerController->GetHUD());

		if (HUD != nullptr)
		{
			UBaseMenuWidget* MenuWidget = Cast<UBaseMenuWidget>(HUD->GetCurrentMenuWidget());
			if (MenuWidget != nullptr)
			{
				MenuWidget->HideCallNotification();
			}
		}
	}
	return FEventReply(false);
}