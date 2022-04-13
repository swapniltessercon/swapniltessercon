// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatrixCommon.h"
#include "CallInfoListWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API UCallInfoListWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UCallInfoListWidget(const FObjectInitializer& ObjectInitializer);
	void HideInforamtionBox();
	void ShowMicInforamtion();
	void CallNotifinctionWindowSetup(EMatrixCallEventType EventType, FString RoomID, FMatrixCallEvent CalledInfo);

private:

	UPROPERTY(meta = (BindWidget))
		class UScrollBox* CallListScrollBox;
	UPROPERTY(meta = (BindWidget))
		class UImage* CallBGImage;
	UPROPERTY(meta = (BindWidget))
		class USizeBox* MicInfoSizeBox;

	TSubclassOf<UUserWidget> CallerNotifyProfileClass;


	UFUNCTION()
		FEventReply OnMouseButtonDownImage(FGeometry MyGeometry, const FPointerEvent& MousEvent);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

};
