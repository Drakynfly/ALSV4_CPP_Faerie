// Project:         Advanced Locomotion System V4 on C++
// Copyright:       Copyright (C) 2021 Doğa Can Yanıkoğlu
// License:         MIT License (http://www.opensource.org/licenses/mit-license.php)
// Source Code:     https://github.com/dyanikoglu/ALSV4_CPP
// Original Author: Doğa Can Yanıkoğlu
// Contributors:    

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Animation/AnimSequenceBase.h"
#include "ALSCharacterEnumLibrary.h"
#include "ALSAnimationStructLibrary.generated.h"

USTRUCT(BlueprintType)
struct FALSDynamicMontageParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Struct Library")
	UAnimSequenceBase* Animation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Struct Library")
	float BlendInTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Struct Library")
	float BlendOutTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Struct Library")
	float PlayRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Struct Library")
	float StartTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FALSLeanAmount
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float LR = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float FB = 0.0f;
};

USTRUCT(BlueprintType)
struct FALSVelocityBlend
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float F = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float B = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float L = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float R = 0.0f;
};

USTRUCT(BlueprintType)
struct FALSTurnInPlaceAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	UAnimSequenceBase* Animation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float AnimatedAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	FName SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	bool ScaleTurnAngle = true;
};

USTRUCT(BlueprintType)
struct FALSAnimCharacterInformation
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FRotator AimingRotation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FRotator CharacterActorRotation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FVector Velocity;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FVector RelativeVelocityDirection;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FVector Acceleration;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FVector MovementInput;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	bool bIsMoving = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	bool bHasMovementInput = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Speed = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float MovementInputAmount = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float AimYawRate = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float ZoomAmount = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	EALSMovementState PrevMovementState = EALSMovementState::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	EALSViewMode ViewMode = EALSViewMode::ThirdPerson;
};


USTRUCT(BlueprintType)
struct FALSAnimGraphGrounded
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Animation Struct Library")
	EALSHipsDirection TrackedHipsDirection = EALSHipsDirection::F;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	bool bShouldMove = false; // Should be false initially

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	bool bRotateL = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	bool bRotateR = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Animation Struct Library")
	bool bPivot = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float RotateRate = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float RotationScale = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float DiagonalScaleAmount = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float WalkRunBlend = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float StandingPlayRate = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float CrouchingPlayRate = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float StrideBlend = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float FYaw = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float BYaw = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float LYaw = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float RYaw = 0.0f;
};

USTRUCT(BlueprintType)
struct FALSAnimGraphInAir
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	bool bJumped = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float JumpPlayRate = 1.2f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float FallSpeed = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float LandPrediction = 1.0f;
};

USTRUCT(BlueprintType)
struct FALSAnimGraphAimingValues
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FRotator SmoothedAimingRotation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FRotator SpineRotation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FVector2D AimingAngle;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float AimSweepTime = 0.5f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float InputYawOffsetTime = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float ForwardYawTime = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float LeftYawTime = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float RightYawTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FALSAnimGraphLayerBlending
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	int32 OverlayOverrideState = 0;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float EnableAimOffset = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float BasePose_N = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float BasePose_CLF = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Arm_L = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Arm_L_Add = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Arm_L_LS = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Arm_L_MS = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Arm_R = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Arm_R_Add = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Arm_R_LS = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Arm_R_MS = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Hand_L = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Hand_R = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Legs = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Legs_Add = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Pelvis = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Pelvis_Add = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Spine = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Spine_Add = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Head = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float Head_Add = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float EnableHandIK_L = 1.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float EnableHandIK_R = 1.0f;
};

USTRUCT(BlueprintType)
struct FALSAnimGraphFootIK
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float FootLock_L_Alpha = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float FootLock_R_Alpha = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	bool UseFootLockCurve_L;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	bool UseFootLockCurve_R;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FVector FootLock_L_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FVector TargetFootLock_R_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FVector FootLock_R_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FRotator TargetFootLock_L_Rotation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FRotator FootLock_L_Rotation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FRotator TargetFootLock_R_Rotation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FRotator FootLock_R_Rotation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FVector FootOffset_L_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FVector FootOffset_R_Location;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FRotator FootOffset_L_Rotation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FRotator FootOffset_R_Rotation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	FVector PelvisOffset;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation Struct Library")
	float PelvisAlpha = 0.0f;
};

USTRUCT(BlueprintType)
struct FALSAnimTurnInPlace
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float TurnCheckMinAngle = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float Turn180Threshold = 130.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float AimYawRateLimit = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float ElapsedDelayTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float MinAngleDelay = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float MaxAngleDelay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	FALSTurnInPlaceAsset N_TurnIP_L90;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	FALSTurnInPlaceAsset N_TurnIP_R90;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	FALSTurnInPlaceAsset N_TurnIP_L180;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	FALSTurnInPlaceAsset N_TurnIP_R180;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	FALSTurnInPlaceAsset CLF_TurnIP_L90;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	FALSTurnInPlaceAsset CLF_TurnIP_R90;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	FALSTurnInPlaceAsset CLF_TurnIP_L180;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	FALSTurnInPlaceAsset CLF_TurnIP_R180;
};

USTRUCT(BlueprintType)
struct FALSAnimRotateInPlace
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float RotateMinThreshold = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float RotateMaxThreshold = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float AimYawRateMinRange = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float AimYawRateMaxRange = 270.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float MinPlayRate = 1.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float MaxPlayRate = 3.0f;
};

USTRUCT(BlueprintType)
struct FALSAnimConfiguration
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float AnimatedWalkSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float AnimatedRunSpeed = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float AnimatedSprintSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float AnimatedCrouchSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float VelocityBlendInterpSpeed = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float GroundedLeanInterpSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float InAirLeanInterpSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float SmoothedAimingRotationInterpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float InputYawOffsetInterpSpeed = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float TriggerPivotSpeedLimit = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float FootHeight = 13.5f;

	/** Threshold value for activating dynamic transition on various animations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float DynamicTransitionThreshold = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float IK_TraceDistanceAboveFoot = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Struct Library")
	float IK_TraceDistanceBelowFoot = 45.0f;
};
