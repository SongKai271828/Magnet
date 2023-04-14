// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "MagnetCharacter.h"
#include "CameraTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class MAGNET_API ACameraTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
public:
	ACameraTriggerBox();
	UFUNCTION()
    void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
    void OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);
	UPROPERTY(EditAnywhere)
	FVector2D NewRotation;
	UPROPERTY(EditAnywhere)
	int CameraPosition;
	UPROPERTY(EditAnywhere)
	double NewOffset;
	UPROPERTY(EditAnywhere)
	double Turning;
	UPROPERTY(EditAnywhere)
	bool StopCurve;
	int used;
	FVector2D OldRotation;
	int OldPosition;
	double OldTurning;
	double OldOffset;
	USplineComponent *CurveRecord;
	int CurveState;
};
