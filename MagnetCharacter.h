// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MagnetCharacter.generated.h"

UCLASS(config=Game)
class AMagnetCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	class UStaticMeshComponent* MagnetTarget;
public:
	AMagnetCharacter();

	FRotator CameraRotation;
	FRotator d_CameraRotation;
	FRotator Turning;
	FVector2D CameraNewLocation;
	FVector2D CameraLocation;
	FVector2D d_CameraLocation;
	FVector Force;
	double CameraDistance;
	double CameraOffset;
	double d_CameraOffset;
	float DelaySeconds;
	float VerticalSeconds;
	int CameraPosition;
	int UpdateTimes;
	int UpdateCnt;
	int TurningTimes;
	int TurningCnt;
	double d_Turning;
	int DelayTimes;
	int DelayCnt;
	bool MouseControl;
	bool UpdateCamera;
	bool EnableLeftRight;
	bool EnableJump;
	int KeyRecord;
	FVector d_TurningLocation;
	FVector OldTurningLocation;
	FRotator OldTurning;
	class USplineComponent* CurveRoute;
	FVector CurveLocation;
	FRotator CurveRotation;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;
	double Height;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void MagnetForce(float Value);
	void MyJump();
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

