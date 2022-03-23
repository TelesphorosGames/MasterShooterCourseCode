// Fill out your copyright notice in the Description page of Project Settings.


#include "TestActorTwo.h"

// Sets default values
ATestActorTwo::ATestActorTwo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestActorTwo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestActorTwo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

