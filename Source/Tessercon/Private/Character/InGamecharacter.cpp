// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/InGamecharacter.h"

#include "UI/InGameMenu/PlayerNamePlateWidget.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "PlayerController/TesserconPlayerInfoComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AInGamecharacter::AInGamecharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;


	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PlayerNameplateComponent = CreateDefaultSubobject<UWidgetComponent>("PlayerNameplateComponent");

	static ConstructorHelpers::FClassFinder<UUserWidget> CharacterNameplateBPObj(TEXT("/Game/BaseCore/Blueprints/Widgets/InGameMenu/PlayerNamePlate_WBP"));
	if (CharacterNameplateBPObj.Class == NULL)
	{
		return;
	}
	CharacterNameplateClass = CharacterNameplateBPObj.Class;

	PlayerNameplateComponent->SetWidgetSpace(EWidgetSpace::Screen);
	PlayerNameplateComponent->SetWidgetClass(CharacterNameplateClass);
	PlayerNameplateComponent->SetRelativeLocation(m_SocketLocation);
	PlayerNameplateComponent->SetDrawAtDesiredSize(true);
	PlayerNameplateComponent->SetupAttachment(GetRootComponent());
	PlayerNameplateComponent->SetVisibility(false);
	PlayerNameplateComponent->InitWidget();
	PlayerInfoComponent = CreateDefaultSubobject<UTesserconPlayerInfoComponent>(TEXT("PlayerInfoComponent"));
}

// Called when the game starts or when spawned
void AInGamecharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInGamecharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	class AInGamecharacter* HitCharacter;

	if (CameraBoom != nullptr && FollowCamera != nullptr && IsLocallyControlled())
	{
		FVector CameraBoomWorldLocation = CameraBoom->GetComponentLocation();
		FRotator FollowCameraWorldRotation = FollowCamera->GetComponentRotation();
		FVector SearchVector = 300 * UKismetMathLibrary::GetForwardVector(FRotator(0, FollowCameraWorldRotation.Yaw, 0));
		FVector EndVector = CameraBoomWorldLocation + SearchVector;

		TArray<AActor*> ActorsToIgnore;
		FHitResult OutHit;
		UKismetSystemLibrary::CapsuleTraceSingle(this, CameraBoomWorldLocation, EndVector, 42, 96, ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHit, true);
		HitCharacter = Cast<AInGamecharacter>(OutHit.GetActor());
		if (m_StoredHitActor == nullptr)
		{
			if (HitCharacter != nullptr && !(HitCharacter->PlayerNameplateComponent->IsVisible()))
			{
				HitCharacter->ShowNamePlate();
			}
		}
		else
		{
			if (HitCharacter != nullptr && HitCharacter != m_StoredHitActor)
			{
				m_StoredHitActor->HideNamePlate();
				HitCharacter->ShowNamePlate();
			}
			else if (HitCharacter == nullptr)
			{
				m_StoredHitActor->HideNamePlate();
			}
		}
		m_StoredHitActor = HitCharacter;
	}
	UPlayerNamePlateWidget* Widget = Cast<UPlayerNamePlateWidget>(PlayerNameplateComponent->GetWidget());
	if (Widget != nullptr)
	{
		Widget->IsSpeechDetected();
	}
}

// Called to bind functionality to input
void AInGamecharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AInGamecharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AInGamecharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AInGamecharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AInGamecharacter::LookUpAtRate);

}

void AInGamecharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AInGamecharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AInGamecharacter::ShowNamePlate()
{
	if (PlayerNameplateComponent == nullptr) return;
	PlayerNameplateComponent->SetVisibility(true);
}

void AInGamecharacter::HideNamePlate()
{
	if (PlayerNameplateComponent == nullptr) return;
	PlayerNameplateComponent->SetVisibility(false);
}

void AInGamecharacter::UpdateNamePlate(const FString& InName, const FString& MatrixUsername, const FString& InUserDesignation, EUserRole InUserRole)
{
	if (PlayerNameplateComponent == nullptr) return;
	UPlayerNamePlateWidget* PlayerNameplateWidget = Cast<UPlayerNamePlateWidget>(PlayerNameplateComponent->GetUserWidgetObject());

	if (PlayerNameplateWidget == nullptr) return;
	PlayerNameplateWidget->UpdateWidgetFields(InName, MatrixUsername, InUserDesignation, InUserRole);
}

void AInGamecharacter::AttemptTeleportInfo(FVector InBoothLocation)
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_SendTeleportPointInfo(InBoothLocation);
	}

	// We are the server and in control of the pawn.
	if (GetRemoteRole() == ROLE_SimulatedProxy)// && GetOwnerRole() == ROLE_Authority)
	{
		UpdateteleportPoint(InBoothLocation);
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		UpdateteleportPoint(InBoothLocation);
	}

	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		//ClientTick(DeltaTime); do nothing
	}
}

void AInGamecharacter::Server_SendTeleportPointInfo_Implementation(FVector InBoothLocation)
{
	UpdateteleportPoint(InBoothLocation);
}

void AInGamecharacter::UpdateteleportPoint(FVector InBoothLocation)
{
	float Var = InBoothLocation.X;
	UE_LOG(LogTemp, Warning, TEXT("User update float Var--------------- %f"), Var);
	int var2 = Var;
	int limit = var2 + 150;

	int Random = (var2 + (rand() % (limit - var2 + 5)));
	float Value = float(Random);

	InBoothLocation.X = Value;
	UE_LOG(LogTemp, Warning, TEXT("User update InBoothLocation.X--------------- %f"), InBoothLocation.X);
	TeleportTo(InBoothLocation, FRotator(0, 0, 0), false, false);
}

void AInGamecharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AInGamecharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
