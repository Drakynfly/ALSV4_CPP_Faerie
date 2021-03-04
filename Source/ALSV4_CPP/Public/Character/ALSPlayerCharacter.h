// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ALSBaseCharacter.h"
#include "ALSPlayerCharacter.generated.h"

class UALSPlayerCameraBehavior;

/**
 *
 */
UCLASS()
class ALSV4_CPP_API AALSPlayerCharacter : public AALSBaseCharacter
{
	GENERATED_BODY()

public:
	AALSPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void OnMovementStateChanged(const EALSMovementState PreviousState) override;
	virtual void OnMovementActionChanged(const EALSMovementAction PreviousAction) override;
	virtual void OnStanceChanged(const EALSStance PreviousStance) override;
	virtual void OnRotationModeChanged(const EALSRotationMode PreviousRotationMode) override;
	virtual void OnGaitChanged(const EALSGait PreviousGait) override;
	virtual void OnViewModeChanged(const EALSViewMode PreviousViewMode) override;

	/** Camera System */

	UFUNCTION(BlueprintGetter, Category = "ALS|Camera System")
    bool IsRightShoulder() const { return bRightShoulder; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
    void SetRightShoulder(bool bNewRightShoulder);

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
    virtual ECollisionChannel GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius);

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
    virtual FTransform GetThirdPersonPivotTarget();

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
    virtual FVector GetFirstPersonCameraTarget();

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
    void GetCameraParameters(float& TPFOVOut, float& FPFOVOut, bool& bRightShoulderOut) const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
    void SetCameraBehavior(UALSPlayerCameraBehavior* CamBeh) { CameraBehavior = CamBeh; }

protected:
	/** Input */

	void PlayerCameraUpInput(float Value);

	void PlayerCameraRightInput(float Value);

	UFUNCTION(BlueprintCallable, Category = "ALS|Input", meta = (DisplayName = "Camera Event"))
	void Input_CameraEvent();

	UFUNCTION(BlueprintCallable, Category = "ALS|Input", meta = (DisplayName = "Camera Event Release"))
	void Input_CameraEvent_Release();

protected:
	/** Input */

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float LookUpDownRate = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float LookLeftRightRate = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float ViewModeSwitchHoldTime = 0.2f;

	/** Camera System */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	float ThirdPersonFOV = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	float FirstPersonFOV = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	bool bRightShoulder = false;

	/** Cached Variables */

	UPROPERTY(BlueprintReadOnly)
	UALSPlayerCameraBehavior* CameraBehavior;

	/** Last time the camera action button is pressed */
	float CameraActionPressedTime = 0.0f;

	/* Timer to manage camera mode swap action */
	FTimerHandle OnCameraModeSwapTimer;
};