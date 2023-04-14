// Copyright Epic Games, Inc. All Rights Reserved.

#include "MagnetCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "MagnetActor.h"

//////////////////////////////////////////////////////////////////////////
// AMagnetCharacter

AMagnetCharacter::AMagnetCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	this->MagnetTarget = nullptr;
	CameraOffset = 400;
	MouseControl = true;
	DelaySeconds = 0.2;
	VerticalSeconds = 0.8;
	CameraRotation = FRotator(0, 0, 0);
	CameraLocation = FVector2D(90, -90);
	CameraDistance = CameraOffset;
	Turning = FRotator(0, 0, 0);
	CurveRoute = nullptr;
	
	EnableLeftRight = false;
	TurningCnt = 0;
	Height = 92.012604;
	
}

void AMagnetCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	Turning.Yaw = GetControlRotation().Yaw;
	UpdateCamera = true;
	CameraNewLocation = FVector2D(60, 180);
	CameraPosition = 0;
	EnableJump = true;
}

void AMagnetCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// AddControllerYawInput(DeltaSeconds);
	if (CurveRoute != nullptr)
	{
		FVector LocationXY0 = this->GetActorLocation();
		double Z = LocationXY0.Z;
		// LocationXY0.Z = 0;
		CurveLocation = CurveRoute->FindLocationClosestToWorldLocation(LocationXY0, ESplineCoordinateSpace::World);
		CurveRotation = CurveRoute->FindRotationClosestToWorldLocation(LocationXY0, ESplineCoordinateSpace::World);
		// AddControllerYawInput(float((CurveRotation.Yaw - Turning.Yaw) / ));
		// double tmp = CurveRotation.Yaw - Turning.Yaw;
		// CameraNewLocation.Y += tmp;
		// if (CameraNewLocation.Y < 0) CameraNewLocation.Y += 360;
		UpdateCamera = true;
		// double tmp = GetControlRotation().Yaw - (CameraLocation.Y - GetControlRotation().Yaw) + (Turning.Yaw - GetControlRotation().Yaw);
		// if (!UpdateCamera && UpdateCnt == 0 && DelayCnt == 0)
		// {
		// 	d_CameraLocation.X = 0;
		// 	UpdateCnt = -10;
		// 	d_CameraLocation.Y = tmp / abs(UpdateCnt);
		// 	d_CameraOffset = 0;
		// }
		Turning.Yaw = CurveRotation.Yaw;
		// FollowCamera->AddRelativeRotation(tmp);
		CurveLocation.Z = Z;
		SetActorLocation(CurveLocation);
	}
	if (UpdateCamera)
	{
		UpdateCamera = false;
		FVector LocC = FollowCamera->GetRelativeLocation() - FVector(400, 0, 0);
		CameraLocation = LocC.UnitCartesianToSpherical() / PI * 180;
		if (CameraLocation.Y < 0) CameraLocation.Y += 360;
		d_CameraLocation = CameraNewLocation - CameraLocation;
		if (CurveRoute != nullptr) d_CameraLocation.Y += CurveRotation.Yaw - GetControlRotation().Yaw;
		// d_CameraLocation = CameraNewLocation - CameraLocation;
		if (d_CameraLocation.Y > 180)
		{
			d_CameraLocation.Y -= 360;
		}
		else if (d_CameraLocation.Y < -180)
		{
			d_CameraLocation.Y += 360;
		}
		double MaxCameraMovement = (abs(d_CameraLocation.Y) > abs(d_CameraLocation.X))? (d_CameraLocation.Y): (d_CameraLocation.X);
		UpdateTimes = abs(round(MaxCameraMovement / 90 * VerticalSeconds / DeltaSeconds));
		if ((CurveRoute != nullptr)) UpdateTimes /= 2;
		if (UpdateTimes < 8) UpdateTimes = 8;
		UpdateCnt = UpdateTimes;
		DelayTimes = round(DelaySeconds / DeltaSeconds);
		DelayCnt = DelayTimes;
		d_CameraLocation /= UpdateTimes;
		d_CameraOffset = (CameraOffset - CameraDistance) / UpdateTimes;
	}
	if (DelayCnt > 0 && CurveRoute == nullptr)
	{
		DelayCnt--;
		return;
	}
	if (UpdateCnt > 0)
	{
		double delta = 0;
		if (UpdateTimes - UpdateCnt <= UpdateTimes / 2)
		{
			delta = (UpdateTimes - UpdateTimes / 4.0f - UpdateCnt) / (UpdateTimes / 4.0f);
		}
		else if (UpdateCnt <= UpdateTimes / 2) {
			delta = (UpdateCnt - UpdateTimes / 4.0f) / (UpdateTimes / 4.0f);
		}
		else {
			delta = 1.0f;
		}
		UpdateCnt--;
		if (CurveRoute != nullptr) {
			if (abs(CameraDistance - CameraOffset) < 0.01) d_CameraOffset = 0;
			delta = 0;
		}
		if (UpdateCnt == 0) EnableLeftRight = true;
		CameraDistance += d_CameraOffset * (1 + delta);
		CameraLocation += d_CameraLocation * (1 + delta);
		FVector NewLocation = FVector(cos(CameraLocation.Y / 180 * PI) * sin(CameraLocation.X / 180 * PI), sin(CameraLocation.Y / 180 * PI) * sin(CameraLocation.X / 180 * PI), cos(CameraLocation.X / 180 * PI));
		NewLocation *= CameraDistance;
		// NewLocation *= 0;
		NewLocation += FVector(400, 0, 0);
		CameraRotation.Yaw = CameraLocation.Y - 180;
		CameraRotation.Pitch = CameraLocation.X - 90;
		FollowCamera->SetRelativeLocation(NewLocation);
		FollowCamera->SetRelativeRotation(CameraRotation);
		
		// GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::Yellow, FString::Printf(TEXT("d_Loc: %f, %f, %f"), CameraNewLocation.Y, Turning.Yaw, CurveRotation.Yaw), false);
	}
	GEngine->AddOnScreenDebugMessage(0, 10.0f, FColor::Yellow, FString::Printf(TEXT("d_Loc: %f, %f, %d, %f, %f"), CameraLocation.Y - GetControlRotation().Yaw - Turning.Yaw, Turning.Yaw, TurningTimes, CameraNewLocation.Y, CameraLocation.Y), false);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMagnetCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	// PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMagnetCharacter::MyJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMagnetCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMagnetCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	// PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMagnetCharacter::TurnAtRate);
	// PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMagnetCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMagnetCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMagnetCharacter::TouchStopped);

	// Magnet Related
	PlayerInputComponent->BindAxis("MagnetForce", this, &AMagnetCharacter::MagnetForce);
}

void AMagnetCharacter::MyJump()
{
	if (!this->EnableJump) return;
	Jump();
}

void AMagnetCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (!this->EnableLeftRight) return;
	Jump();
}

void AMagnetCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMagnetCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	// AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMagnetCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	// AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMagnetCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		if (TurningTimes && JumpCurrentCount > 0)
		{
			// JumpCurrentCount = 0;
			// JumpCurrentCountPreJump = 0;
			const FRotator Rotation = Turning;
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			const FVector DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(DirectionX, Value * -1);
			return;
		}
		if (TurningTimes) TurningCnt += (Value * KeyRecord > 0) ? 1 : -1;
		if (TurningTimes && TurningCnt > TurningTimes)
		{
			TurningCnt = TurningTimes + 1;
		}
		if (TurningTimes && TurningCnt < 0)
		{
			TurningCnt = -1;
		}
		// if (TurningTimes && this->GetActorLocation().Z > Height + d_TurningLocation.Z + 15)
		// {
		// 	Value = 0;
		// }
		if (TurningTimes)
		{
			Value *= 0.8;
			Turning.Yaw = OldTurning.Yaw + d_Turning * ((TurningCnt > TurningTimes) ? TurningTimes : ((TurningCnt < 0) ? 0: TurningCnt));
		}
		if (Turning.Yaw > 180) Turning.Yaw -= 360;
		else if (Turning.Yaw < -180) Turning.Yaw += 360;

		const FRotator Rotation = Turning;
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(DirectionX, Value);

		if (TurningTimes && TurningCnt <= TurningTimes && TurningCnt >= 0)
		{
			FVector tmp = this->GetActorLocation();
			for (int i = 0; i < 2; i++)
			{
				if (abs(this->d_TurningLocation.Component(i)) < 5000)
				{
					tmp.Component(i) = this->OldTurningLocation.Component(i) + TurningCnt * this->d_TurningLocation.Component(i);
				}
				else if (Value < 0)
				{
					tmp.Component(i) += (this->OldTurningLocation.Component(i) - tmp.Component(i)) / TurningTimes * (TurningTimes - TurningCnt);
				}
			}
			// tmp.Z -= (tmp.Z - Height - this->d_TurningLocation.Z) / TurningTimes * (TurningTimes - TurningCnt);
			SetActorLocation(tmp);
		}
		
		if (TurningTimes && KeyRecord == 0)
		// if (TurningTimes && (TurningCnt >= TurningTimes || TurningTimes && TurningCnt <= 0) && KeyRecord == 0)
		{
			if (TurningCnt < TurningTimes / 2) TurningCnt = 0;
			else TurningCnt = TurningTimes;
			Turning.Yaw = OldTurning.Yaw + d_Turning * ((TurningCnt > TurningTimes) ? TurningTimes : ((TurningCnt < 0) ? 0: TurningCnt));
			TurningTimes = 0;
			TurningCnt = 0;
		}
	}
}

void AMagnetCharacter::MoveRight(float Value)
{
	if (!EnableLeftRight && !CurveRoute) return;
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Turning;
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		double CameraRotationYaw = CameraLocation.Y - GetControlRotation().Yaw - Turning.Yaw;
		if (CameraRotationYaw < -269.9) CameraRotationYaw += 360;
		else if (CameraRotationYaw > 269.9) CameraRotationYaw -= 360;
	
		// get right vector
		const FVector DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector DirectionY = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		if (CameraRotationYaw > -180 && CameraRotationYaw < 0)
		{
			AddMovementInput(DirectionX, Value);
		}
		else if (CameraRotationYaw > 0 && CameraRotationYaw < 180)
		{
			AddMovementInput(DirectionX, -1 * Value);
		}
		else
		{
			// AddMovementInput(DirectionY, Value);
		}
	}
}

void AMagnetCharacter::MagnetForce(float Value)
{
	FCollisionQueryParams MagnetFeild;
	MagnetFeild.bTraceComplex = true;
    MagnetFeild.bReturnPhysicalMaterial = false;
    MagnetFeild.AddIgnoredActor(this);
	FHitResult hitresult(ForceInit);
	FVector forward = GetActorForwardVector();
	forward.Normalize();
	FVector position = this->GetActorLocation();
	FVector boundary = position + forward * 500;
	GetWorld()->LineTraceSingleByChannel(hitresult, position, boundary, ECC_WorldStatic, MagnetFeild);
	if (hitresult.GetActor() && Cast<AMagnetActor>(hitresult.GetActor()))
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, hitresult.GetActor()->GetName() );
		DrawDebugLine(this->GetWorld(), position, boundary, FColor::Black, true, 5.0f);
		this->MagnetTarget = Cast<UStaticMeshComponent>((hitresult.GetActor())->GetRootComponent());
		Force = forward * 200000 * Value;
		// Force.X = 0;
		// Force.Y = 0;
		// Force.Z = 200000;
		// FVector Force(0, 0, 1000000);
		this->MagnetTarget->AddForce(Force);
	}
	else
	{
		this->MagnetTarget = nullptr;
	}
}

