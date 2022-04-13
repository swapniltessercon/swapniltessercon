// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/BaseCharacter.h"
#include "TesserconCoreCommon.h"
#include "InGamecharacter.generated.h"

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
		FString Name;

	UPROPERTY(BlueprintReadOnly)
		FString MatrixUsername;

	UPROPERTY(BlueprintReadOnly)
		FString UserDesignation;

	UPROPERTY(BlueprintReadOnly)
		EUserRole UserRole;

	FPlayerInfo()
	{
		Name = "";
		MatrixUsername = "";
		UserDesignation = "";
		UserRole = EUserRole::Attendee;
	}

	FPlayerInfo(const FString& InName, const FString& InMatrixUsername, const FString& InUserDesignation, EUserRole InUserRole)
	{
		Name = InName;
		MatrixUsername = InMatrixUsername;
		UserDesignation = InUserDesignation;
		UserRole = InUserRole;
	}
};


UCLASS()
class TESSERCON_API AInGamecharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AInGamecharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerNameTag, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* PlayerNameplateComponent;

	UPROPERTY(BlueprintReadOnly)
		class UTesserconPlayerInfoComponent* PlayerInfoComponent;

	UFUNCTION(BlueprintCallable)
		void ShowNamePlate();
	UFUNCTION(BlueprintCallable)
		void HideNamePlate();

	TSubclassOf<UUserWidget> CharacterNameplateClass;
	class UCharacterNameplateWidget* PlayerNameplate;

	void UpdateNamePlate(const FString& InName, const FString& MatrixUsername, const FString& InUserDesignation, EUserRole InUserRole);
	FVector m_SocketLocation;

	UFUNCTION(Server, Reliable)
		void Server_SendTeleportPointInfo(FVector InBoothLocation);

	void AttemptTeleportInfo(FVector InBoothLocation);

	void UpdateteleportPoint(FVector InBoothLocation);

	AInGamecharacter* m_StoredHitActor;



protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

};
