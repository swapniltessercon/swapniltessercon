// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterSelectActor.generated.h"

UCLASS()
class TESSERCON_API ACharacterSelectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacterSelectActor();

	virtual void Tick(float DeltaTime) override;

	// Set SkeletalMesh And Animation using Gender and AvatarIndex 
	void SetSkeletalMeshAndAnim();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* DefaultComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* SkeletalMesh;



};
