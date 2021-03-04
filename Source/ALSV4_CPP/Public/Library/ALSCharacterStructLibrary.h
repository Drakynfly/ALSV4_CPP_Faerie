// Project:         Advanced Locomotion System V4 on C++
// Copyright:       Copyright (C) 2021 Doğa Can Yanıkoğlu
// License:         MIT License (http://www.opensource.org/licenses/mit-license.php)
// Source Code:     https://github.com/dyanikoglu/ALSV4_CPP
// Original Author: Doğa Can Yanıkoğlu
// Contributors:


#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Library/ALSCharacterEnumLibrary.h"

#include "ALSCharacterStructLibrary.generated.h"

class UCurveVector;
class UAnimMontage;
class UAnimSequenceBase;
class UCurveFloat;

USTRUCT(BlueprintType)
struct FALSComponentAndTransform
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FTransform Transform;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	class UPrimitiveComponent* Component = nullptr;
};

USTRUCT(BlueprintType)
struct FALSCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float TargetArmLength = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FVector SocketOffset;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float LagSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float RotationLagSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	bool bDoCollisionTest = true;
};

USTRUCT(BlueprintType)
struct FALSCameraGaitSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSCameraSettings Walking;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSCameraSettings Running;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSCameraSettings Sprinting;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSCameraSettings Crouching;
};

USTRUCT(BlueprintType)
struct FALSCameraStateSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSCameraGaitSettings VelocityDirection;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSCameraGaitSettings LookingDirection;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSCameraGaitSettings Aiming;
};

USTRUCT(BlueprintType)
struct FALSMantleAsset : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	UAnimMontage* AnimMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	UCurveVector* PositionCorrectionCurve = nullptr;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FVector StartingOffset;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float LowHeight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float LowPlayRate = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float LowStartPosition = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float HighHeight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float HighPlayRate = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float HighStartPosition = 0.0f;
};

USTRUCT(BlueprintType)
struct FALSMantleParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	UAnimMontage* AnimMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	UCurveVector* PositionCorrectionCurve = nullptr;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float StartingPosition = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float PlayRate = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FVector StartingOffset;
};

USTRUCT(BlueprintType)
struct FALSMantleTraceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float MaxLedgeHeight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float MinLedgeHeight = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float ReachDistance = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float ForwardTraceRadius = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float DownwardTraceRadius = 0.0f;
};

USTRUCT(BlueprintType)
struct FALSMovementSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float SlowSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float NormalSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float FastSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	UCurveVector* MovementCurve = nullptr;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	UCurveFloat* RotationRateCurve = nullptr;

	float GetSpeedForGait(const EALSGait Gait) const
	{
		switch (Gait)
		{
		case EALSGait::Normal: return NormalSpeed;
		case EALSGait::Fast: return FastSpeed;
		case EALSGait::Slow: return SlowSpeed;
		default: return NormalSpeed;
		}
	}
};

USTRUCT(BlueprintType)
struct FALSMovementStanceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSMovementSettings Standing;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSMovementSettings Crouching;


	// These are not stances, per se, but they fix the best here. I do not like this, but the other option is to have a
	// new struct or many more entries in FALSMovementStateSettings. I consider this a workable solution only becuase
	// this struct isnt used anywhere else.

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSMovementSettings Flying;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSMovementSettings Swimming;
};

USTRUCT(BlueprintType)
struct FALSMovementStateSettings : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSMovementStanceSettings VelocityDirection;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSMovementStanceSettings LookingDirection;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FALSMovementStanceSettings Aiming;
};

USTRUCT(BlueprintType)
struct FALSRotateInPlaceAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	UAnimSequenceBase* Animation = nullptr;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	FName SlotName;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float SlowTurnRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float FastTurnRate = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float SlowPlayRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Character Struct Library")
	float FastPlayRate = 1.0f;
};