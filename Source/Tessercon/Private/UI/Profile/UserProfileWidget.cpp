// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Profile/UserProfileWidget.h"

//DownLoading 
#include "Blueprint/AsyncTaskDownloadImage.h"
//class

#include "UI/MainMenu/ProfileBarWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "UI/MainMenu/CallInfoListWidget.h"

//Components
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"


void UUserProfileWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ProfileCloseButton != nullptr)
	{
		ProfileCloseButton->OnClicked.AddDynamic(this, &UUserProfileWidget::OnProfileCloseButtonClicked);
	}
	if (ProfileEditAvatarButton != nullptr)
	{
		ProfileEditAvatarButton->OnClicked.AddDynamic(this, &UUserProfileWidget::OnProfileEditAvatarButtonClicked);
	}
	if (ProfileChatButton != nullptr)
	{
		ProfileChatButton->OnClicked.AddDynamic(this, &UUserProfileWidget::OnProfileChatButtonClicked);
	}
	if (ProfileCallButton != nullptr)
	{
		ProfileCallButton->OnClicked.AddDynamic(this, &UUserProfileWidget::OnProfileCallButtonClicked);
	}
	if (RequestBusinesCardButton != nullptr)
	{
		RequestBusinesCardButton->OnClicked.AddDynamic(this, &UUserProfileWidget::OnRequestBusinesCardButtonClicked);
	}
	if (ProfileBlockButton != nullptr)
	{
		ProfileBlockButton->OnClicked.AddDynamic(this, &UUserProfileWidget::OnProfileBlockButtonClicked);
	}
	if (BGBlurImage != nullptr)
	{
		BGBlurImage->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDownImage"));
	}

	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();

		TessClient->Getsession().d_EventOnMicAvailablityChanged.AddUObject(this, &UUserProfileWidget::OnMicAvailablityChanged);
		TessClient->Getsession().d_EventOnSpeakerAvailablityChanged.AddUObject(this, &UUserProfileWidget::OnSpeakerAvailablityChanged);

		//Check Mic Available Or Not
		if (!TessClient->Getsession().IsMicAvailable())
		{
			MicNotifyIconImage->SetVisibility(ESlateVisibility::Visible);
			ProfileCallButton->SetIsEnabled(false);
		}
		else
		{
			MicNotifyIconImage->SetVisibility(ESlateVisibility::Collapsed);
			ProfileCallButton->SetIsEnabled(true);
		}

	}
	
}

void UUserProfileWidget::NativeDestruct()
{
	if (FModuleManager::Get().IsModuleLoaded("TesserconCore"))
	{
		TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
		TessClient->Getsession().d_EventOnMicAvailablityChanged.RemoveAll(this);
		TessClient->Getsession().d_EventOnSpeakerAvailablityChanged.RemoveAll(this);
	}
	Super::NativeDestruct();
}


void UUserProfileWidget::SetProfile(FTesserconUserInfo InUserInfo, FString Sender)
{
	UserID = Sender;
	UserIds.Add(Sender);
	if (FModuleManager::Get().IsModuleLoaded("MatrixCore"))
	{
		p_ChatClient = FModuleManager::Get().GetModulePtr<FMatrixCoreModule>("MatrixCore")->GetMatrixClient();
		m_RoomID = p_ChatClient->GetSession().GetRoomIDForFriend(UserID);
		TMap<FString, TSharedPtr<MatrixRoomSession>> FriendRoomSession = p_ChatClient->GetSession().GetFriendRooms();

		if (FriendRoomSession.Find(m_RoomID))
		{
			ProfileCallButton->SetIsEnabled(true);
		}
		else
		{
			ProfileCallButton->SetIsEnabled(false);
		}

	}
	SetProfile(InUserInfo);
}




void UUserProfileWidget::OnSpeakerAvailablityChanged(bool bIsSpeakerAvailable)
{

	if (!bIsSpeakerAvailable)
	{
		MicNotifyIconImage->SetVisibility(ESlateVisibility::Visible);
		ProfileCallButton->SetIsEnabled(false);
	}
	else
	{
		MicNotifyIconImage->SetVisibility(ESlateVisibility::Collapsed);
		ProfileCallButton->SetIsEnabled(true);
	}

}
void UUserProfileWidget::OnMicAvailablityChanged(bool bIsMicAvailable)
{

	if (!bIsMicAvailable)
	{
		MicNotifyIconImage->SetVisibility(ESlateVisibility::Visible);
		ProfileCallButton->SetIsEnabled(true);
	}
	else
	{
		MicNotifyIconImage->SetVisibility(ESlateVisibility::Collapsed);
		ProfileCallButton->SetIsEnabled(true);
	}

}

void UUserProfileWidget::SetProfile(FTesserconUserInfo InUserInfo)
{
	
	UserInfo = InUserInfo;
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		LoggedInUserInfo = TessClient->Getsession().GetLoggedInUserInfo();
	}
	if (LoggedInUserInfo.Username == InUserInfo.Username)
	{
		SetCommonProfile(InUserInfo);
		ProfileBlockButton->SetVisibility(ESlateVisibility::Collapsed);
		RequestBussinessCardSizeBox->SetVisibility(ESlateVisibility::Collapsed);
	    BusinessCardDetailsSizeBox->SetVisibility(ESlateVisibility::Collapsed);
		ChatAndCallHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
		
	}
	else
	{
		SetCommonProfile(InUserInfo);
		BusinessCardDetailsSizeBox->SetVisibility(ESlateVisibility::Collapsed);
		AvatarEditButtonSizeBox->SetVisibility(ESlateVisibility::Collapsed);
	}
}


void UUserProfileWidget::SetCommonProfile(FTesserconUserInfo InUserInfo)
{
	FString Name = InUserInfo.FirstName + " " + InUserInfo.LastName;
	UserNameTextBlock->SetText(FText::FromString(Name));
	DesignationTextBlock->SetText(FText::FromString(InUserInfo.Designation));
	AboutTextBlock->SetText(FText::FromString(InUserInfo.Bio));
	CompantyNameTextBlock->SetText(FText::FromString(InUserInfo.CompanyName));
	CompanyWebsiteTextBlock->SetText(FText::FromString(InUserInfo.Website));

	if (ProfileImage != nullptr)
	{
		UAsyncTaskDownloadImage* DownloadedImageTask = NewObject<UAsyncTaskDownloadImage>();
		DownloadedImageTask->OnSuccess.AddDynamic(this, &UUserProfileWidget::OnSetProfileTexture);
		DownloadedImageTask->Start(InUserInfo.ProfileImageURL);
	}
	
}

void UUserProfileWidget::OnSetProfileTexture(UTexture2DDynamic* Texture)
{
	if (ProfileImage == nullptr) return;
	ProfileImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	ProfileImage->SetBrushFromTextureDynamic(Texture);
}


void UUserProfileWidget::OnProfileCloseButtonClicked()
{
	RemoveFromViewport();
	UE_LOG(LogTemp, Warning, TEXT("OnProfileCloseButtonClicked"));
}

void UUserProfileWidget::OnProfileEditAvatarButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnProfileEditAvatarButtonClicked"))
}


void UUserProfileWidget::OnProfileChatButtonClicked()
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
				FString RoomID = p_ChatClient->GetSession().GetRoomIDForFriend(UserID);
				TMap<FString, TSharedPtr<MatrixRoomSession>> FriendRoomSession = p_ChatClient->GetSession().GetFriendRooms();
				TSubclassOf<UUserWidget> ProfileBarClass;
				MenuWidget->GetProfileBarClass(ProfileBarClass);
				UProfileBarWidget* ProfileRow = CreateWidget<UProfileBarWidget>(GetWorld(), ProfileBarClass);
				if (ProfileRow != nullptr)
				{
					if (FriendRoomSession.Find(RoomID))
					{
						ProfileRow->ShowChatWidget(UserInfo, RoomID);
						this->RemoveFromParent();
					}
					else
					{
						if (p_ChatClient != nullptr)
						{
							MatrixNativeAPI::FOnMatrixCreateRoomReponseDelegate CreateRoomCallBack;
							CreateRoomCallBack.BindLambda([this, ProfileRow, RoomID](EMatrixCoreError Error, FString RoomId)
								{
									UE_LOG(LogTemp, Warning, TEXT("MatrixNativeAPI::FOnMatrixCreateRoomReponseDelegate"));
									if (Error == EMatrixCoreError::Success)
									{
										UserIds.Empty();
										ProfileRow->ShowChatWidget(UserInfo, RoomID);
										this->RemoveFromParent();

									}
								});
							p_ChatClient->GetSession().CreateRoom(UserIds, true, "", CreateRoomCallBack);
						}
					}
				}
			}
		}

	}
	UE_LOG(LogTemp, Warning, TEXT("OnProfileChatButtonClicked"))
}



// call 
void UUserProfileWidget::OnProfileCallButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnProfileCallButtonClicked"))
	TesserconClient* TessClient = FModuleManager::Get().GetModulePtr<FTesserconCoreModule>("TesserconCore")->GetTesserconClient();
	if (TessClient != nullptr)
	{
		TessClient->Getsession().PlaceCall(m_RoomID);
	}
}



void UUserProfileWidget::OnRequestBusinesCardButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRequestBusinesCardButtonClicked"));
}


void UUserProfileWidget::OnProfileBlockButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnProfileBlockButtonClicked"))
}


//
FEventReply UUserProfileWidget::OnMouseButtonDownImage(FGeometry MyGeometry, const FPointerEvent& MousEvent)
{
	RemoveFromParent();
	return FEventReply(false);
}