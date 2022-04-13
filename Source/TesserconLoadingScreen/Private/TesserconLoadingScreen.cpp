// Copyright Epic Games, Inc. All Rights Reserved.

#include "TesserconLoadingScreen.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "MoviePlayer.h"
#include "Widgets/Images/SThrobber.h"
#include "Engine/GameEngine.h"
#include "EngineGlobals.h"

#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( FPaths::GameSourceDir() / "GUI" / RelativePath + TEXT(".ttf"), __VA_ARGS__ ) 

// This module must be loaded "PreLoadingScreen" in the .uproject file, otherwise it will not hook in time!
struct FTesserconLoadingScreenBrush : public FSlateDynamicImageBrush, public FGCObject
{
	FTesserconLoadingScreenBrush(const FName InTextureName, const FVector2D& InImageSize)
		: FSlateDynamicImageBrush(InTextureName, InImageSize)
	{
		SetResourceObject(LoadObject<UObject>(NULL, *InTextureName.ToString()));
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector)
	{
		if (UObject* CachedResourceObject = GetResourceObject())
		{
			Collector.AddReferencedObject(CachedResourceObject);
		}
	}
};

class STesserconLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STesserconLoadingScreen) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs)
	{
		// Load version of the logo with text baked in, path is hardcoded because this loads very early in startup
		static const FName TesserconLogo(TEXT("/Game/BaseCore/Textures/LoadingScreen/LoadingTesserconLogo"));
		static const FName LoadingScreen(TEXT("/Game/BaseCore/Textures/LoadingScreen/loadingBG"));
		static const FName InfoVector(TEXT("/Game/BaseCore/Textures/LoadingScreen/LoadingInfoVector"));
		static const FName VivoxLogo(TEXT("/Game/BaseCore/Textures/LoadingScreen/LoadingVivoxLogo"));
		static const FName UnrealLogo(TEXT("/Game/BaseCore/Textures/LoadingScreen/Loadingunreallogo"));
		static const FName MatrixLogo(TEXT("/Game/BaseCore/Textures/LoadingScreen/Loadingmatrixlogo"));

		TesserconLogoBrush = MakeShareable(new FTesserconLoadingScreenBrush(TesserconLogo, FVector2D(187,103)));
		VivoxLogoBrush= MakeShareable(new FTesserconLoadingScreenBrush(VivoxLogo, FVector2D(75, 67)));
		LoadingBackGroundBrush = MakeShareable(new FTesserconLoadingScreenBrush(LoadingScreen, FVector2D(1920, 1080)));
		UnrealLogoBrush = MakeShareable(new FTesserconLoadingScreenBrush(UnrealLogo, FVector2D(41, 41)));
		MatrixLogoBrush = MakeShareable(new FTesserconLoadingScreenBrush(MatrixLogo, FVector2D(84, 36)));
		InfoVectorBrush = MakeShareable(new FTesserconLoadingScreenBrush(InfoVector, FVector2D(1022, 14)));


		ChildSlot
			[

				SNew(SOverlay)
				+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage)
				.Image(LoadingBackGroundBrush.Get())
			]
		+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(0, 0, 0, 155)
			[
				SNew(SImage)
				.Image(TesserconLogoBrush.Get())
			]
		+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(0, 0, 0,80)
			[
				SNew(SImage)
				.Image(InfoVectorBrush.Get())
			]
			+SOverlay::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Bottom)
			.Padding(10,0,0,12)
			[
				SNew(SImage)
				.Image(VivoxLogoBrush.Get())
			]

			+ SOverlay::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Bottom)
			.Padding(90,0,0,30)
			[
				SNew(SImage)
				.Image(UnrealLogoBrush.Get())
			]

			+ SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Bottom)
				.Padding(145, 0, 0,30)
				[
					SNew(SImage)
					.Image(MatrixLogoBrush.Get())
				]
			+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(0, 0, 0, 0)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Curabitur vulputate arcu odio, ac facilisis diam accumsan ut.\n Ut imperdiet et leo in vulputate.")))
					//./*Font(TTF_FONT("Poppins-Light_Font", 14))*/
				    .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 15))
				]
			//+ SOverlay::Slot()
			//.VAlign(VAlign_Bottom)
			//.HAlign(HAlign_Right)
			//.Padding(0, 0, 50, 28)
			//[
			//	SNew(SVerticalBox)
			//	+ SVerticalBox::Slot()
			//	.VAlign(VAlign_Bottom)
			//	.HAlign(HAlign_Right)
			//	//.Padding(FMargin(5))
			//	.Padding(5.0f)
			//	[
			//		SNew(SThrobber)
			//		.Visibility(this, &STesserconLoadingScreen::GetLoadIndicatorVisibility)
		
			//	]
			//]
			+ SOverlay::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Right)
			.Padding(0, 0, 120, 30)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("CONNECTING ...")))
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 15))
			]
	
		];
	}

private:
	/** Rather to show the ... indicator */
	EVisibility GetLoadIndicatorVisibility() const
	{
		bool Vis = GetMoviePlayer()->IsLoadingFinished();
		return GetMoviePlayer()->IsLoadingFinished() ? EVisibility::Collapsed : EVisibility::Visible;
	}

	/** Loading screen image brush */
	TSharedPtr<FSlateDynamicImageBrush> TesserconLogoBrush;
	TSharedPtr<FSlateDynamicImageBrush> LoadingBackGroundBrush;
	TSharedPtr<FSlateDynamicImageBrush> VivoxLogoBrush;
	TSharedPtr<FSlateDynamicImageBrush> UnrealLogoBrush;
	TSharedPtr<FSlateDynamicImageBrush> MatrixLogoBrush;
	TSharedPtr<FSlateDynamicImageBrush> InfoVectorBrush;
};

class FTesserconLoadingScreenModule : public ITesserconLoadingScreenModule
{
public:
	virtual void StartupModule() override
	{
		// Force load for cooker reference
	//	LoadObject<UObject>(nullptr, TEXT("/Game/BaseCore/Textures/LoadingScreen/loadingBG") );

		if (IsMoviePlayerEnabled())
		{
			//CreateScreen();
		}
	}


	virtual bool IsGameModule() const override
	{
		return true;
	}

	virtual void StartInGameLoadingScreen(bool bPlayUntilStopped, float PlayTime) override
	{
	
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = !bPlayUntilStopped;
		LoadingScreen.bWaitForManualStop = bPlayUntilStopped;
		LoadingScreen.bAllowEngineTick = bPlayUntilStopped;
		LoadingScreen.MinimumLoadingScreenDisplayTime = PlayTime;
		LoadingScreen.WidgetLoadingScreen = SNew(STesserconLoadingScreen);
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}

	virtual void StopInGameLoadingScreen() override
	{
		GetMoviePlayer()->StopMovie();
	}

	virtual void CreateScreen()
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
		LoadingScreen.MinimumLoadingScreenDisplayTime = 5.f;
		LoadingScreen.WidgetLoadingScreen = SNew(STesserconLoadingScreen);
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}

};

IMPLEMENT_GAME_MODULE(FTesserconLoadingScreenModule, TesserconLoadingScreen);