// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "JumpOffTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class MAGNET_API AJumpOffTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
public:
	AJumpOffTriggerBox();
	UFUNCTION()
    void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);
	bool used;
};
