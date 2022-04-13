// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "CharacterSelector.generated.h"

/**
 * 
 */
UCLASS()
class TESSERCON_API ACharacterSelector : public ABaseCharacter
{
	GENERATED_BODY()

public:
    void SetSkeletalMeshAndAnim();



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* SkeletalMesh;

};
