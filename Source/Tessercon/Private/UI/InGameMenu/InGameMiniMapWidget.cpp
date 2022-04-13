// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenu/InGameMiniMapWidget.h"
#include "UI/HUD/InGameHUD.h"

#include "Kismet/KismetMaterialLibrary.h"

void  UInGameMiniMapWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInGameMiniMapWidget::SetupMap()
{
	AInGameHUD* HUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (HUD != nullptr)
	{
		m_Zoom = HUD->m_MapZoom;
		m_Dimension = HUD->m_MapDimension;
		p_MiniMapImage = HUD->m_MapImage;
	}
}
