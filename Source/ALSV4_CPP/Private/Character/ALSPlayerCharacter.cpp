// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ALSPlayerCharacter.h"
#include "Character/Animation/ALSPlayerCameraBehavior.h"

AALSPlayerCharacter::AALSPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AALSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward/Backwards", this, &AALSPlayerCharacter::PlayerForwardMovementInput);
	PlayerInputComponent->BindAxis("MoveRight/Left", this, &AALSPlayerCharacter::PlayerRightMovementInput);
	PlayerInputComponent->BindAxis("LookUp/Down", this, &AALSPlayerCharacter::PlayerCameraUpInput);
	PlayerInputComponent->BindAxis("LookLeft/Right", this, &AALSPlayerCharacter::PlayerCameraRightInput);
}

void AALSPlayerCharacter::OnMovementStateChanged(const EALSMovementState PreviousState)
{
	Super::OnMovementStateChanged(PreviousState);

	if (CameraBehavior)
	{
		CameraBehavior->MovementState = MovementState;
	}
}

void AALSPlayerCharacter::OnMovementActionChanged(const EALSMovementAction PreviousAction)
{
	Super::OnMovementActionChanged(PreviousAction);

	if (CameraBehavior)
	{
		CameraBehavior->MovementAction = MovementAction;
	}
}

void AALSPlayerCharacter::OnStanceChanged(const EALSStance PreviousStance)
{
	Super::OnStanceChanged(PreviousStance);

	if (CameraBehavior)
	{
		CameraBehavior->Stance = Stance;
	}
}

void AALSPlayerCharacter::OnRotationModeChanged(const EALSRotationMode PreviousRotationMode)
{
	Super::OnRotationModeChanged(PreviousRotationMode);

	if (CameraBehavior)
	{
		CameraBehavior->SetRotationMode(RotationMode);
	}
}

void AALSPlayerCharacter::OnGaitChanged(const EALSGait PreviousGait)
{
	if (CameraBehavior)
	{
		CameraBehavior->Gait = Gait;
	}
}

void AALSPlayerCharacter::SetRightShoulder(const bool bNewRightShoulder)
{
	bRightShoulder = bNewRightShoulder;
	if (CameraBehavior)
	{
		CameraBehavior->bRightShoulder = bRightShoulder;
	}
}

void AALSPlayerCharacter::OnViewModeChanged(const EALSViewMode PreviousViewMode)
{
	Super::OnViewModeChanged(PreviousViewMode);
	if (CameraBehavior)
	{
		CameraBehavior->ViewMode = ViewMode;
	}
}

ECollisionChannel AALSPlayerCharacter::GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius)
{
	TraceOrigin = GetActorLocation();
	TraceRadius = 10.0f;
	return ECC_Visibility;
}

FTransform AALSPlayerCharacter::GetThirdPersonPivotTarget()
{
	return GetActorTransform();
}

FVector AALSPlayerCharacter::GetFirstPersonCameraTarget()
{
	return GetMesh()->GetSocketLocation(FName(TEXT("FP_Camera")));
}

void AALSPlayerCharacter::GetCameraParameters(float& TPFOVOut, float& FPFOVOut, bool& bRightShoulderOut) const
{
	TPFOVOut = ThirdPersonFOV;
	FPFOVOut = FirstPersonFOV;
	bRightShoulderOut = bRightShoulder;
}

void AALSPlayerCharacter::PlayerCameraUpInput(const float Value)
{
	AddControllerPitchInput(LookUpDownRate * Value);
}

void AALSPlayerCharacter::PlayerCameraRightInput(const float Value)
{
	AddControllerYawInput(LookLeftRightRate * Value);
}

void AALSPlayerCharacter::Input_CameraEvent()
{
	UWorld* World = GetWorld();
	check(World);
	CameraActionPressedTime = World->GetTimeSeconds();
	GetWorldTimerManager().SetTimer(OnCameraModeSwapTimer, this,
                                    &AALSPlayerCharacter::OnSwitchCameraMode, ViewModeSwitchHoldTime, false);
}

void AALSPlayerCharacter::Input_CameraEvent_Release()
{
	if (ViewMode == EALSViewMode::FirstPerson)
	{
		// Don't swap shoulders on first person mode
		return;
	}

	UWorld* World = GetWorld();
	check(World);
	if (World->GetTimeSeconds() - CameraActionPressedTime < ViewModeSwitchHoldTime)
	{
		// Switch shoulders
		SetRightShoulder(!bRightShoulder);
		GetWorldTimerManager().ClearTimer(OnCameraModeSwapTimer); // Prevent mode change
	}
}