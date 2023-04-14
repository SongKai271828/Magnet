// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraTriggerBox.h"
#include "MagnetCharacter.h"

ACameraTriggerBox::ACameraTriggerBox()
{
    //Register Events
    OnActorBeginOverlap.AddDynamic(this, &ACameraTriggerBox::OnOverlapBegin);
    OnActorEndOverlap.AddDynamic(this, &ACameraTriggerBox::OnOverlapEnd);
    used = 0;
    CurveRecord = nullptr;
    CurveState = 0;
}

// Called when the game starts or when spawned
void ACameraTriggerBox::BeginPlay()
{
	Super::BeginPlay();
    if (this->StopCurve)
    {
        FVector Scale(1, 1, 10);
        this->SetActorScale3D(Scale);
    }
    else
    {
        FVector Scale(2.5, 2.5, 10);
        this->SetActorScale3D(Scale);
    }
}

void ACameraTriggerBox::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
    // check if Actors do not equal nullptr and that 
    class AMagnetCharacter* character_ptr = Cast<AMagnetCharacter>(OtherActor);
    if (character_ptr == nullptr) return;
    // GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::Yellow, FString::Printf(TEXT("d_Loc: %d %f %f"), this->used, this->OldTurning, character_ptr->Turning.Yaw), false);
    if (this->StopCurve)
    {
        if (character_ptr->CurveRoute != nullptr)
        {
            if (CurveState == 0) CurveRecord = character_ptr->CurveRoute;
            character_ptr->CurveRoute = nullptr;
            CurveState = 1;
        }
        else if (character_ptr->CurveRoute == nullptr && CurveRecord != nullptr && CurveState == 1)
        {
            character_ptr->CurveRoute = CurveRecord;
            CurveState = -1;
        }
        // FVector tmp = character_ptr->GetActorLocation();
        // tmp.Z = this->GetActorLocation().Z + character_ptr->Height;
        // character_ptr->SetActorLocation(tmp);
    }
    
    if (this->used == 0)
    {
        this->used = 1;
        this->OldRotation = character_ptr->CameraNewLocation;
        this->OldTurning = character_ptr->Turning.Yaw;
        this->OldPosition = character_ptr->CameraPosition;
        this->OldOffset = character_ptr->CameraDistance;
        character_ptr->CameraOffset = this->NewOffset;
    }
    
    character_ptr->OldTurningLocation = character_ptr->GetActorLocation();
    FVector tmp1 = this->GetActorLocation();
    FVector tmp2 = character_ptr->OldTurningLocation * 0;
    character_ptr->OldTurning = character_ptr->Turning;
    character_ptr->EnableLeftRight = false;
    if (character_ptr->CurveRoute != nullptr) return;

    if (this->used == 1 && abs(this->Turning - character_ptr->Turning.Yaw) > 1 || this->used == -1 && abs(this->OldTurning - character_ptr->Turning.Yaw) < 1)
    {
        this->used = 1;
        character_ptr->TurningTimes = 20;
        character_ptr->TurningCnt = 0;
        character_ptr->KeyRecord = 1;
        // character_ptr->Turning.Yaw = this->Turning;
        character_ptr->d_Turning = this->Turning - character_ptr->Turning.Yaw;
        if (character_ptr->d_Turning > 269) character_ptr->d_Turning -= 360;
        else if (character_ptr->d_Turning < -269) character_ptr->d_Turning += 360;
        character_ptr->d_Turning /= character_ptr->TurningTimes;
        if (abs(this->Turning - 90) < 1 || abs(this->Turning + 90) < 1)
        {
            tmp2.X = tmp1.X - character_ptr->OldTurningLocation.X;
            // tmp2.Y = tmp2.X;
            tmp2 /= character_ptr->TurningTimes;
            tmp2.Y = 10000;
        }
        else if (abs(this->Turning - 0) < 1 || abs(this->Turning - 180) < 1)
        {
            tmp2.Y = tmp1.Y - character_ptr->OldTurningLocation.Y;
            // tmp2.X = tmp2.Y;
            tmp2 /= character_ptr->TurningTimes;
            tmp2.X = 10000;
        }
    }
    else if (this->used == -1 && abs(this->OldTurning - character_ptr->Turning.Yaw) > 1)
    {
        character_ptr->TurningTimes = 20;
        character_ptr->TurningCnt = 0;
        character_ptr->KeyRecord = -1;
        // character_ptr->Turning.Yaw = this->OldTurning;
        character_ptr->d_Turning = this->OldTurning - character_ptr->Turning.Yaw;
        if (character_ptr->d_Turning > 269) character_ptr->d_Turning -= 360;
        else if (character_ptr->d_Turning < -269) character_ptr->d_Turning += 360;
        character_ptr->d_Turning /= character_ptr->TurningTimes;
        if (abs(this->OldTurning - 90) < 1 || abs(this->OldTurning + 90) < 1)
        {
            tmp2.X = tmp1.X - character_ptr->OldTurningLocation.X;
            // tmp2.Y = tmp2.X;
            tmp2 /= character_ptr->TurningTimes;
            tmp2.Y = 10000;
        }
        else if (abs(this->OldTurning - 0) < 1 || abs(this->OldTurning - 180) < 1)
        {
            tmp2.Y = tmp1.Y - character_ptr->OldTurningLocation.Y;
            // tmp2.X = tmp2.Y;
            tmp2 /= character_ptr->TurningTimes;
            tmp2.X = 10000;
        }
    }
    tmp2.Z = tmp1.Z;
    character_ptr->d_TurningLocation = tmp2;
}

void ACameraTriggerBox::OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor)
{
    class AMagnetCharacter* character_ptr = Cast<AMagnetCharacter>(OtherActor);
    if (character_ptr == nullptr) return;
    double ActorTuringYaw = (character_ptr->GetActorRotation()).Yaw;
    // GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::Yellow, FString::Printf(TEXT("ActorTuringYaw: %f, %f, %f"), ActorTuringYaw, this->OldTurning, this->Turning), false);
    if (this->used == 1)
    {
        if (abs(ActorTuringYaw - this->Turning) < 90 || character_ptr->CurveRoute != nullptr)
        {
            this->used = -1;
            if (character_ptr->CameraPosition != this->CameraPosition)
            {
                character_ptr->UpdateCamera = true;
                if (this->CameraPosition == 1)
                {
                    character_ptr->CameraNewLocation = FVector2D(60, 270);
                }
                else if (this->CameraPosition == 2)
                {
                    character_ptr->CameraNewLocation = FVector2D(60, 90);
                }
                else if (this->CameraPosition == 3)
                {
                    character_ptr->CameraNewLocation = FVector2D(5, 180);
                }
                else if (this->CameraPosition == 5)
                {
                    character_ptr->CameraNewLocation = this->NewRotation;
                }
                else if (this->CameraPosition == 4)
                {
                    character_ptr->CameraNewLocation = FVector2D(60, 0);
                }
                else if (this->CameraPosition == 0)
                {
                    character_ptr->CameraNewLocation = FVector2D(60, 180);
                }
                character_ptr->CameraPosition = this->CameraPosition;
            }
        }
        else //if (((int)round(abs(ActorTuringYaw - this->Turning) / 90)) % 2)
        {
            // GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::Yellow, FString::Printf(TEXT("d_Loc: %d, %d"), 1, 1), false);
            this->used = 1;
            // character_ptr->Turning.Yaw = this->OldTurning;
        }
    }
    else if (this->used == -1)
    {
        GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::Yellow, FString::Printf(TEXT("d_Loc: %d, %d"), -1, 1), false);
        if (abs(ActorTuringYaw - this->Turning) < 90 && character_ptr->CurveRoute == nullptr)
        {
            // character_ptr->Turning.Yaw = this->Turning;
        }
        else //if (((int)round(abs(ActorTuringYaw - this->Turning) / 90)) % 2)
        {
            if (character_ptr->CameraPosition != this->OldPosition)
            {
                
                character_ptr->UpdateCamera = true;
                character_ptr->CameraNewLocation = this->OldRotation;
                character_ptr->CameraPosition = this->OldPosition;
                character_ptr->CameraOffset = this->OldOffset;
            }
            this->used = 1;
        }
    }
    // FVector actorlocation = character_ptr->GetActorLocation();
    // actorlocation.Z = this->GetActorLocation().Z;
    // this->SetActorLocation(actorlocation);
    // character_ptr->EnableJump = true;
    // character_ptr->TurningTimes = 0;
    character_ptr->KeyRecord = 0;
}
