// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterSelectActor.h"

// Sets default values
ACharacterSelectActor::ACharacterSelectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	DefaultComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultComponent"));
	AddOwnedComponent(DefaultComponent);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	AddOwnedComponent(SkeletalMesh);
}

// Called when the game starts or when spawned
void ACharacterSelectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterSelectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void ACharacterSelectActor::SetSkeletalMeshAndAnim()
{

}

