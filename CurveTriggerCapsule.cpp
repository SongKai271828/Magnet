// Fill out your copyright notice in the Description page of Project Settings.


#include "CurveTriggerCapsule.h"
#include "MagnetCharacter.h"
#include "Components/SplineComponent.h"

ACurveTriggerCapsule::ACurveTriggerCapsule()
{
    //Register Events
    OnActorBeginOverlap.AddDynamic(this, &ACurveTriggerCapsule::OnOverlapBegin);
    CurveRoute = CreateDefaultSubobject<USplineComponent>(TEXT("CurveRoute"));
    CurveRoute->SetupAttachment(RootComponent);
    used = false;
}
 
// Called when the game starts or when spawned
void ACurveTriggerCapsule::BeginPlay()
{
	Super::BeginPlay();
}

void ACurveTriggerCapsule::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
    class AMagnetCharacter* character_ptr = Cast<AMagnetCharacter>(OtherActor);
    if (character_ptr == nullptr) return;
    // GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::Yellow, FString::Printf(TEXT("d_Loc: %d %f %f"), 8, 8.8, 8.8), false);
    if (!character_ptr->CurveRoute)
    {
        used = true;
        character_ptr->CurveRoute = this->CurveRoute;
        OldRotation = character_ptr->CameraNewLocation;
        OldTurning = character_ptr->Turning.Yaw;
        OldPosition = character_ptr->CameraPosition;
        OldOffset = character_ptr->CameraOffset;
    }
    else
    {
        used = false;
        character_ptr->CurveRoute = nullptr;
        character_ptr->CameraPosition = OldPosition;
        character_ptr->CameraNewLocation = OldRotation;
        character_ptr->Turning.Yaw = OldTurning;
        character_ptr->CameraOffset = OldOffset;
        character_ptr->UpdateCamera = true;
    }
}


