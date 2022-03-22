// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Data/ALSMovementSettingsPreset.h"
#include "Library/ALSCharacterEnumLibrary.h"
#include "Library/ALSCharacterStructLibrary.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"

#include "ALSBaseCharacter.generated.h"

// forward declarations
class UALSDebugComponent;
class UAnimMontage;
class UALSPlayerCameraBehavior;
enum class EVisibilityBasedAnimTickOption : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpPressedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRagdollStateChangedSignature, bool, bRagdollState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FViewModeChangedSignature, class ACharacter*, Character, EALSViewMode, PrevViewMode);

/*
 * Base character class
 */
UCLASS(BlueprintType)
class ALSV4_CPP_API AALSBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AALSBaseCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "ALS|Movement")
	FORCEINLINE class UALSCharacterMovementComponent* GetMyMovementComponent() const
	{
		return MyCharacterMovementComponent;
	}

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
					FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
					const FHitResult& Hit) override;

	// We are overriding this to implement custom handling for flight logic.
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce = false) override;

	/** Ragdoll System */

	/** Implement on BP to get required get up animation according to character's state */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "ALS|Ragdoll System")
	UAnimMontage* GetGetUpAnimation(bool bRagdollFaceUpState);

	UFUNCTION(BlueprintCallable, Category = "ALS|Ragdoll System")
	virtual void RagdollStart();

	UFUNCTION(BlueprintCallable, Category = "ALS|Ragdoll System")
	virtual void RagdollEnd();

	UFUNCTION(BlueprintCallable, Server, Unreliable, Category = "ALS|Ragdoll System")
	void Server_SetMeshLocationDuringRagdoll(FVector MeshLocation);

	/** Character States */

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetMovementState(EALSMovementState NewState, bool bForce = false);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EALSMovementState GetMovementState() const { return MovementState; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EALSMovementState GetPrevMovementState() const { return PrevMovementState; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetMovementAction(EALSMovementAction NewAction, bool bForce = false);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EALSMovementAction GetMovementAction() const { return MovementAction; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetStance(EALSStance NewStance, bool bForce = false);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EALSStance GetStance() const { return Stance; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetOverlayOverrideState(int32 NewState);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	int32 GetOverlayOverrideState() const { return OverlayOverrideState; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetGait(EALSGait NewGait, bool bForce = false);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EALSGait GetGait() const { return Gait; }

	UFUNCTION(BlueprintGetter, Category = "ALS|CharacterStates")
	EALSGait GetDesiredGait() const { return DesiredGait; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetRotationMode(EALSRotationMode NewRotationMode, bool bForce = false);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_SetRotationMode(EALSRotationMode NewRotationMode, bool bForce);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EALSRotationMode GetRotationMode() const { return RotationMode; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetViewMode(EALSViewMode NewViewMode, bool bForce = false);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_SetViewMode(EALSViewMode NewViewMode, bool bForce);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EALSViewMode GetViewMode() const { return ViewMode; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetFlightState(EALSFlightState NewFlightState, bool bForce = false);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_SetFlightState(EALSFlightState NewFlightState, bool bForce);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EALSFlightState GetFlightState() const { return FlightState; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetOverlayState(EALSOverlayState NewState, bool bForce = false);

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetGroundedEntryState(EALSGroundedEntryState NewState);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_SetOverlayState(EALSOverlayState NewState, bool bForce);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EALSOverlayState GetOverlayState() const { return OverlayState; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	EALSGroundedEntryState GetGroundedEntryState() const { return GroundedEntryState; }

	/** Landed, Jumped, Rolling, Mantling and Ragdoll*/
	/** On Landed*/
	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void EventOnLanded();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "ALS|Character States")
	void Multicast_OnLanded();

	/** On Jumped*/
	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void EventOnJumped();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "ALS|Character States")
	void Multicast_OnJumped();

	/** Rolling Montage Play Replication*/
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_PlayMontage(UAnimMontage* Montage, float PlayRate);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "ALS|Character States")
	void Multicast_PlayMontage(UAnimMontage* Montage, float PlayRate);

	/** Ragdolling */
	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void ReplicatedRagdollStart();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_RagdollStart();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "ALS|Character States")
	void Multicast_RagdollStart();

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void ReplicatedRagdollEnd();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_RagdollEnd(FVector CharacterLocation);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "ALS|Character States")
	void Multicast_RagdollEnd(FVector CharacterLocation);

	/** Input */

	UPROPERTY(BlueprintAssignable, Category = "ALS|Input")
	FJumpPressedSignature JumpPressedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "ALS|Input")
	FOnJumpedSignature OnJumpedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "ALS|Input")
	FRagdollStateChangedSignature RagdollStateChangedDelegate;

	UFUNCTION(BlueprintGetter, Category = "ALS|Input")
	EALSStance GetDesiredStance() const { return DesiredStance; }

	UFUNCTION(BlueprintSetter, Category = "ALS|Input")
	void SetDesiredStance(EALSStance NewStance);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Input")
	void Server_SetDesiredStance(EALSStance NewStance);

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetDesiredGait(EALSGait NewGait);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_SetDesiredGait(EALSGait NewGait);

	UFUNCTION(BlueprintGetter, Category = "ALS|Input")
	EALSRotationMode GetDesiredRotationMode() const { return DesiredRotationMode; }

	UFUNCTION(BlueprintSetter, Category = "ALS|Input")
	void SetDesiredRotationMode(EALSRotationMode NewRotMode);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_SetDesiredRotationMode(EALSRotationMode NewRotMode);

	/** Rotation System */

	UFUNCTION(BlueprintCallable, Category = "ALS|Rotation System")
	void SetActorLocationAndTargetRotation(FVector NewLocation, FRotator NewRotation);

	/** Movement System */

	UFUNCTION(BlueprintGetter, Category = "ALS|Movement System")
	bool HasMovementInput() const { return bHasMovementInput; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	FALSMovementSettings GetTargetMovementSettings() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	void AddMovementModifier(const FALSMovementModifier Modifier);

	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	void RemoveMovementModifier(const FName ModifierID);

	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	void SetMovementModifierTime(const FName ModifierID, const float Time);

	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	EALSGait GetAllowedGait() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	EALSGait GetActualGait(EALSGait AllowedGait) const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	bool CanSprint() const;

	/** BP implementable function that is called when a Breakfall starts */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Movement System")
	void OnBreakfall();
	virtual void OnBreakfall_Implementation();

	/** BP implementable function that called when A Montage starts, e.g. during rolling */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Movement System")
	void Replicated_PlayMontage(UAnimMontage* Montage, float PlayRate);
	virtual void Replicated_PlayMontage_Implementation(UAnimMontage* Montage, float PlayRate);

	/** Implement on BP to get required roll animation according to character's state */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "ALS|Movement System")
	UAnimMontage* GetRollAnimation();

	/** Flight System */

	/** This can be overriden to setup custom conditions for allowing character flight. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "ALS|Flight")
	virtual bool CanFly() const;

	/** This can be overriden to setup custom conditions for allowing character flight from blueprint. */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure = false, Category = "ALS|Flight")
	bool FlightCheck() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintPure = false, Category = "ALS|Flight")
	bool FlightInterruptCheck(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
								bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
								const FHitResult& Hit) const;

	/** Utility */

	UFUNCTION(BlueprintCallable, Category = "ALS|Utility")
	float GetAnimCurveValue(FName CurveName) const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Utility")
	void SetVisibleMesh(USkeletalMesh* NewVisibleMesh);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Utility")
	void Server_SetVisibleMesh(USkeletalMesh* NewSkeletalMesh);

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

	/** Essential Information Getters/Setters */

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	FVector GetAcceleration() const { return Acceleration; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	bool IsMoving() const { return bIsMoving; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Essential Information")
	FVector GetMovementInput() const;

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	float GetMovementInputAmount() const { return MovementInputAmount; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	float GetSpeed() const { return Speed; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Essential Information")
	FRotator GetAimingRotation() const { return AimingRotation; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	float GetAimYawRate() const { return AimYawRate; }

	/** Input */

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void ForwardMovementAction(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void RightMovementAction(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void UpMovementAction(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void CameraUpAction(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void CameraRightAction(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void JumpAction(bool bValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void SprintAction(bool bValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void AimAction(bool bValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void CameraTapAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void CameraHeldAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void StanceAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void WalkAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void RagdollAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void VelocityDirectionAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void LookingDirectionAction();

protected:
	/** Ragdoll System */

	void RagdollUpdate(float DeltaTime);

	void SetActorLocationDuringRagdoll(float DeltaTime);

	/** State Changes */

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual void OnMovementStateChanged(EALSMovementState PreviousState);

	virtual void OnMovementActionChanged(EALSMovementAction PreviousAction);

	virtual void OnStanceChanged(EALSStance PreviousStance);

	virtual void OnRotationModeChanged(EALSRotationMode PreviousRotationMode);

	virtual void OnFlightStateChanged(EALSFlightState PreviousFlightState);

	virtual void OnGaitChanged(EALSGait PreviousGait);

	virtual void OnViewModeChanged(EALSViewMode PreviousViewMode);

	virtual void OnOverlayStateChanged(EALSOverlayState PreviousState);

	virtual void OnVisibleMeshChanged(const USkeletalMesh* PreviousSkeletalMesh);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnJumped_Implementation() override;

	virtual void Landed(const FHitResult& Hit) override;

	void OnLandFrictionReset();

	void SetEssentialValues(float DeltaTime);

	void UpdateCharacterMovement();

	void UpdateFlightMovement(float DeltaTime);

	void UpdateGroundedRotation(float DeltaTime);

	void UpdateFallingRotation(float DeltaTime);

	void UpdateFlightRotation(float DeltaTime);

	void UpdateSwimmingRotation(float DeltaTime);

	/** Utils */

	// Gets the relative altitude of the player, measuring down to a point below the character.
	UFUNCTION(BlueprintCallable, Category = "ALS|Flight")
	float FlightDistanceCheck(float CheckDistance, FVector Direction) const;

	void SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed, float DeltaTime);

	float CalculateGroundedRotationRate() const;

	float CalculateFlightRotationRate() const;
	void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed, float DeltaTime);

	void UpdateRelativeAltitude();

	void ForceUpdateCharacterState();

	/** Replication */
	UFUNCTION(Category = "ALS|Replication")
	void OnRep_RotationMode(EALSRotationMode PrevRotMode);

	UFUNCTION(Category = "ALS|Replication")
	void OnRep_FlightState(EALSFlightState PrevFlightState);

	UFUNCTION(Category = "ALS|Replication")
	void OnRep_ViewMode(EALSViewMode PrevViewMode);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnViewModeChanged", ScriptName = "OnViewModeChanged"))
	void K2_OnViewModeChanged(EALSViewMode PreviousViewMode);

	UFUNCTION(Category = "ALS|Replication")
	void OnRep_OverlayState(EALSOverlayState PrevOverlayState);

	UFUNCTION(Category = "ALS|Replication")
	void OnRep_VisibleMesh(USkeletalMesh* NewVisibleMesh);

public:
	/** Multicast delegate for ViewMode changing. */
	UPROPERTY(BlueprintAssignable, Category=Character)
	FViewModeChangedSignature ViewModeChangedDelegate;

protected:
	/* Custom movement component*/
	UPROPERTY()
	TObjectPtr<UALSCharacterMovementComponent> MyCharacterMovementComponent;

	/** Input */

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "ALS|Input")
	EALSRotationMode DesiredRotationMode = EALSRotationMode::LookingDirection;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "ALS|Input")
	EALSGait DesiredGait = EALSGait::Running;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "ALS|Input")
	EALSStance DesiredStance = EALSStance::Standing;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float LookUpDownRate = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float LookLeftRightRate = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float RollDoubleTapTimeout = 0.3f;

	UPROPERTY(Category = "ALS|Input", BlueprintReadOnly)
	bool bBreakFall = false;

	UPROPERTY(Category = "ALS|Input", BlueprintReadOnly)
	bool bSprintHeld = false;

	/** Camera System */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	float ThirdPersonFOV = 90.0f;

	/** Flight */

	// Flag to call CanFly() per tick. This will disable flight in midair on a false return. When disabled, CanFly() will
	// only be called on pre-takeoff.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS|Flight")
	bool AlwaysCheckFlightConditions = false;

	// Should FlightInterruptCheck be used to determine if flight should be interrupted when colliding in midair.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS|Flight")
	bool UseFlightInterrupt = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	float FirstPersonFOV = 90.0f;

	// The velocity of the hit required to trigger a positive FlightInterruptThresholdCheck.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ALS|Flight")
	float FlightInterruptThreshold = 600;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	bool bRightShoulder = false;

	// The max degree that flight input will consider forward.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Flight", Meta = (UIMin = 0, UIMax = 90))
	float MaxFlightForwardAngle = 85;
	/** Movement System */

	// Maximum rotation in Yaw, Pitch and Roll, that may be achieved in flight.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|Flight")
	FVector MaxFlightLean = {40, 40, 0};

	/** Essential Information */

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	FVector Acceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	bool bHasMovementInput = false;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	FRotator LastVelocityRotation;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	FRotator LastMovementInputRotation;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float MovementInputAmount = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float AimYawRate = 0.0f;

	/** Replicated Essential Information*/

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float EasedMaxAcceleration = 0.0f;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ALS|Essential Information")
	FVector ReplicatedCurrentAcceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ALS|Essential Information")
	FRotator ReplicatedControlRotation = FRotator::ZeroRotator;

	/** Replicated Skeletal Mesh Information*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Skeletal Mesh", ReplicatedUsing = OnRep_VisibleMesh)
	TObjectPtr<USkeletalMesh> VisibleMesh = nullptr;

	/** State Values */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|State Values", ReplicatedUsing = OnRep_OverlayState)
	EALSOverlayState OverlayState = EALSOverlayState::Default;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	EALSGroundedEntryState GroundedEntryState;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	EALSMovementState MovementState = EALSMovementState::None;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	EALSMovementState PrevMovementState = EALSMovementState::None;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	EALSMovementAction MovementAction = EALSMovementAction::None;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values", ReplicatedUsing = OnRep_RotationMode)
	EALSRotationMode RotationMode = EALSRotationMode::LookingDirection;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values", ReplicatedUsing = OnRep_FlightState)
	EALSFlightState FlightState = EALSFlightState::None;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	EALSGait Gait = EALSGait::Walking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|State Values")
	EALSStance Stance = EALSStance::Standing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|State Values", ReplicatedUsing = OnRep_ViewMode)
	EALSViewMode ViewMode = EALSViewMode::ThirdPerson;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	int32 OverlayOverrideState = 0;

	/** Movement System */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|Movement System")
	TObjectPtr<UALSMovementSettingsPreset> MovementData;

	/** Rotation System */

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Rotation System")
	FRotator TargetRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Rotation System")
	FRotator InAirRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Rotation System")
	float YawOffset = 0.0f;

	/** Breakfall System */

	/** If player hits to the ground with a specified amount of velocity, switch to breakfall state */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Breakfall System")
	bool bBreakfallOnLand = true;

	/** If player hits to the ground with an amount of velocity greater than specified value, switch to breakfall state */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Breakfall System", meta = (EditCondition ="bBreakfallOnLand"))
	float BreakfallOnLandVelocity = 700.0f;

	/** Flag to tell the breakfall system to activate the next time the character lands. This will set to false immediately after. */
	UPROPERTY(BlueprintReadWrite, Category = "ALS|Breakfall System")
	bool bBreakFallNextLanding = false;

	/** Ragdoll System */

	/** If the skeleton uses a reversed pelvis bone, flip the calculation operator */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Ragdoll System")
	bool bReversedPelvis = false;

	/** If player hits to the ground with a specified amount of velocity, switch to ragdoll state */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Ragdoll System")
	bool bRagdollOnLand = false;

	/** If player hits to the ground with an amount of velocity greater than specified value, switch to ragdoll state */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Ragdoll System", meta = (EditCondition ="bRagdollOnLand"))
	float RagdollOnLandVelocity = 1000.0f;

	/** Is the current ragdoll state grounded */
	UPROPERTY(BlueprintReadOnly, Category = "ALS|Ragdoll System")
	bool bRagdollOnGround = false;

	/** If player starts to freefall while rolling, switch to ragdoll state */
	UPROPERTY(BlueprintReadWrite, Category = "ALS|Ragdoll System")
	bool bRagdollOnRollfall = false;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Ragdoll System")
	bool bRagdollFaceUp = false;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Ragdoll System")
	FVector LastRagdollVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ALS|Ragdoll System")
	FVector TargetRagdollLocation = FVector::ZeroVector;

	/* Server ragdoll pull force storage*/
	float ServerRagdollPull = 0.0f;

	/* Dedicated server mesh default visibility based anim tick option*/
	EVisibilityBasedAnimTickOption DefVisBasedTickOp;

	bool bPreRagdollURO = false;

	/** Cached Variables */

	FVector PreviousVelocity = FVector::ZeroVector;

	float PreviousAimYaw = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Camera")
	TObjectPtr<UALSPlayerCameraBehavior> CameraBehavior;

	// Altitude variables for flight calculations.
	float SeaAltitude = 0; // @todo essentially global.
	float TroposphereHeight = 0; // @todo essentially global.

	float RelativeAltitude = 0;
	float AbsoluteAltitude = 0;

	float AtmosphereAtAltitude = 1;

	TArray<FALSMovementModifier> MovementModifiers;

	/** Last time the 'first' crouch/roll button is pressed */
	float LastStanceInputTime = 0.0f;

	/* Timer to manage reset of braking friction factor after on landed event */
	FTimerHandle OnLandedFrictionResetTimer;

	/* Smooth out aiming by interping control rotation*/
	FRotator AimingRotation = FRotator::ZeroRotator;

	/** We won't use curve based movement and a few other features on networked games */
	bool bEnableNetworkOptimizations = false;

private:
	UPROPERTY()
	TObjectPtr<UALSDebugComponent> ALSDebugComponent = nullptr;
};