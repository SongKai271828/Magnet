// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpOffTriggerBox.h"
#include "MagnetCharacter.h"

AJumpOffTriggerBox::AJumpOffTriggerBox()
{
    //Register Events
    OnActorBeginOverlap.AddDynamic(this, &AJumpOffTriggerBox::OnOverlapBegin);
    used = false;
}
 
// Called when the game starts or when spawned
void AJumpOffTriggerBox::BeginPlay()
{
	Super::BeginPlay();
    FVector Scale(0.1, 0.1, 10);
    this->SetActorScale3D(Scale);
}

void AJumpOffTriggerBox::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
    class AMagnetCharacter* character_ptr = Cast<AMagnetCharacter>(OtherActor);
    if (character_ptr->EnableJump)
    {
        character_ptr->EnableJump = false;
    }
    else
    {
        character_ptr->EnableJump = true;
    }
}

