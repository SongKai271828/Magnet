// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerCapsule.h"
#include "MagnetCharacter.h"
#include "Components/SplineComponent.h"
#include "CurveTriggerCapsule.generated.h"

/**
 * 
 */
UCLASS()
class MAGNET_API ACurveTriggerCapsule : public ATriggerCapsule
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USplineComponent* CurveRoute;
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
public:
	ACurveTriggerCapsule();
	UFUNCTION()
    void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);
	bool used;
	FVector2D OldRotation;
	int OldPosition;
	double OldTurning;
	double OldOffset;
};
