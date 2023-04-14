// Fill out your copyright notice in the Description page of Project Settings.


#include "MagnetActor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AMagnetActor::AMagnetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// this->StaticMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMagnetActor::BeginPlay()
{
	Super::BeginPlay();
	this->StaticMesh = Cast<UStaticMeshComponent>(this->GetRootComponent());
	// StaticMesh->SetSimulatePhysics(true);
}

// Called every frame
void AMagnetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector forward(0, 0, 1);
	// FVector forward = this->GetActorForwardVector();
	// forward.Normalize();
	forward *= 500000;
	// this->StaticMesh->AddForce(forward);
	// this->StaticMesh->AddRadialImpulse(GetActorLocation(), 500.f, 2000.f, ERadialImpulseFalloff::RIF_Constant, true);
	// GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::Yellow, FString::Printf(TEXT("d_Loc: %f, %f"), 0.0f, 0.0f), false);
}

