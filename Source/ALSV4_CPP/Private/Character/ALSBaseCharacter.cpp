// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#include "Character/ALSBaseCharacter.h"

#include "ALSStaticNames.h"
#include "ALS_Settings.h"
#include "Character/Animation/ALSCharacterAnimInstance.h"
#include "Character/Animation/ALSPlayerCameraBehavior.h"
#include "Components/ALSDebugComponent.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveFloat.h"
#include "Character/ALSCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

using namespace ALS::BaseCharacter;

AALSBaseCharacter::AALSBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UALSCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = 0;
	bReplicates = true;
	SetReplicatingMovement(true);

	const UALS_Settings* UALS_Settings = UALS_Settings::Get();
	SeaAltitude = UALS_Settings->SeaAltitude;
	TroposphereHeight = UALS_Settings->TroposphereHeight;
}

void AALSBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MyCharacterMovementComponent = Cast<UALSCharacterMovementComponent>(Super::GetMovementComponent());
}

void AALSBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AALSBaseCharacter, TargetRagdollLocation);
	DOREPLIFETIME_CONDITION(AALSBaseCharacter, ReplicatedCurrentAcceleration, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AALSBaseCharacter, ReplicatedControlRotation, COND_SkipOwner);

	DOREPLIFETIME(AALSBaseCharacter, DesiredGait);
	DOREPLIFETIME_CONDITION(AALSBaseCharacter, DesiredStance, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AALSBaseCharacter, DesiredRotationMode, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(AALSBaseCharacter, RotationMode, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AALSBaseCharacter, OverlayState, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AALSBaseCharacter, FlightState, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AALSBaseCharacter, ViewMode, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AALSBaseCharacter, VisibleMesh, COND_SkipOwner);
}

void AALSBaseCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
                                  const bool bSelfMoved, const FVector HitLocation, const FVector HitNormal,
                                  const FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (FlightState != EALSFlightState::None)
	{
		if ((UseFlightInterrupt && FlightInterruptCheck(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit))
			|| RelativeAltitude <= 5.f)
		{
			SetFlightState(EALSFlightState::None);
		}
	}
}

void AALSBaseCharacter::AddMovementInput(FVector WorldDirection, const float ScaleValue, const bool bForce)
{
	if (GetCharacterMovement()->MovementMode == MOVE_Flying)
	{
		// Prevent the player from flying above world max height.
		if (WorldDirection.Z > 0.0f) { WorldDirection.Z *= AtmosphereAtAltitude; }
	}

	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

void AALSBaseCharacter::OnBreakfall_Implementation()
{
	Replicated_PlayMontage(GetRollAnimation(), 1.35);
}

void AALSBaseCharacter::Replicated_PlayMontage_Implementation(UAnimMontage* Montage, const float PlayRate)
{
	// Roll: Simply play a Root Motion Montage.
	if (GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);
	}

	Server_PlayMontage(Montage, PlayRate);
}

void AALSBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// If we're in networked game, disable curved movement
	bEnableNetworkOptimizations = !IsNetMode(NM_Standalone);

	// Make sure the mesh and AnimBP update after the CharacterBP to ensure it gets the most recent values.
	GetMesh()->AddTickPrerequisiteActor(this);

	// Force update states to use the initial desired values.
	ForceUpdateCharacterState();

	if (Stance == EALSStance::Standing)
	{
		UnCrouch();
	}
	else if (Stance == EALSStance::Crouching)
	{
		Crouch();
	}

	// Set default rotation values.
	TargetRotation = GetActorRotation();
	LastVelocityRotation = TargetRotation;
	LastMovementInputRotation = TargetRotation;

	if (GetMesh()->GetAnimInstance() && GetLocalRole() == ROLE_SimulatedProxy)
	{
		GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	}

	MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());

	ALSDebugComponent = FindComponentByClass<UALSDebugComponent>();
}

void AALSBaseCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Set required values
	SetEssentialValues(DeltaTime);

	switch (MovementState)
	{
		case EALSMovementState::None: break;
		case EALSMovementState::Grounded:
			{
				// Cancel movement and rotation when riding.
				if (Stance == EALSStance::Riding) break;

				UpdateCharacterMovement();
				UpdateGroundedRotation(DeltaTime);
				break;
			}
		case EALSMovementState::Freefall:	UpdateFallingRotation(DeltaTime); break;
		case EALSMovementState::Flight:
			{
				UpdateRelativeAltitude();
				UpdateCharacterMovement();
				UpdateFlightRotation(DeltaTime);

				//@todo why is this here? should probably be in CharacterMovementComponent
				// or flight component
				if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
				{
					UpdateFlightMovement(DeltaTime);
				}
				break;
			}
		case EALSMovementState::Swimming:
			{
				UpdateCharacterMovement();
				UpdateSwimmingRotation(DeltaTime);
				break;
			}
		case EALSMovementState::Mantling:	break;
		case EALSMovementState::Ragdoll:	RagdollUpdate(DeltaTime); break;
		default: break;
	}

	// Cache values
	PreviousVelocity = GetVelocity();
	PreviousAimYaw = AimingRotation.Yaw;
}

void AALSBaseCharacter::RagdollStart()
{
	if (RagdollStateChangedDelegate.IsBound())
	{
		RagdollStateChangedDelegate.Broadcast(true);
	}

	/** When Networked, disables replicate movement reset TargetRagdollLocation and ServerRagdollPull variable
	and if the host is a dedicated server, change character mesh optimisation option to avoid z-location bug*/
	MyCharacterMovementComponent->bIgnoreClientMovementErrorChecksAndCorrection = 1;

	if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
	{
		DefVisBasedTickOp = GetMesh()->VisibilityBasedAnimTickOption;
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	}
	TargetRagdollLocation = GetMesh()->GetSocketLocation(NAME_Pelvis);
	ServerRagdollPull = 0;

	// Disable URO
	bPreRagdollURO = GetMesh()->bEnableUpdateRateOptimizations;
	GetMesh()->bEnableUpdateRateOptimizations = false;

	// Step 1: Clear the Character Movement Mode and set the Movement State to Ragdoll
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	SetMovementState(EALSMovementState::Ragdoll);

	// Step 2: Disable capsule collision and enable mesh physics simulation starting from the pelvis.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(NAME_Pelvis, true, true);

	// Step 3: Stop any active montages.
	if (GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.2f);
	}

	// Fixes character mesh is showing default A pose for a split-second just before ragdoll ends in listen server games
	GetMesh()->bOnlyAllowAutonomousTickPose = true;

	SetReplicateMovement(false);
}

void AALSBaseCharacter::RagdollEnd()
{
	/** Re-enable Replicate Movement and if the host is a dedicated server set mesh visibility based anim
	tick option back to default*/

	if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
	{
		GetMesh()->VisibilityBasedAnimTickOption = DefVisBasedTickOp;
	}

	GetMesh()->bEnableUpdateRateOptimizations = bPreRagdollURO;

	// Revert back to default settings
	MyCharacterMovementComponent->bIgnoreClientMovementErrorChecksAndCorrection = 0;
	GetMesh()->bOnlyAllowAutonomousTickPose = false;
	SetReplicateMovement(true);

	// Step 1: Save a snapshot of the current Ragdoll Pose for use in AnimGraph to blend out of the ragdoll
	if (GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->SavePoseSnapshot(NAME_RagdollPose);
	}

	// Step 2: If the ragdoll is on the ground, set the movement mode to walking and play a Get Up animation.
	// If not, set the movement mode to falling and update the character movement velocity to match the last ragdoll velocity.
	if (bRagdollOnGround)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		if (GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(GetGetUpAnimation(bRagdollFaceUp), 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
		}
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->Velocity = LastRagdollVelocity;
	}

	// Step 3: Re-Enable capsule collision, and disable physics simulation on the mesh.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetAllBodiesSimulatePhysics(false);

	if (RagdollStateChangedDelegate.IsBound())
	{
		RagdollStateChangedDelegate.Broadcast(false);
	}
}

void AALSBaseCharacter::Server_SetMeshLocationDuringRagdoll_Implementation(const FVector MeshLocation)
{
	TargetRagdollLocation = MeshLocation;
}

void AALSBaseCharacter::SetMovementState(const EALSMovementState NewState, const bool bForce)
{
	if (bForce || MovementState != NewState)
	{
		PrevMovementState = MovementState;
		MovementState = NewState;
		OnMovementStateChanged(PrevMovementState);
	}
}

void AALSBaseCharacter::SetMovementAction(const EALSMovementAction NewAction, const bool bForce)
{
	if (bForce || MovementAction != NewAction)
	{
		const EALSMovementAction Prev = MovementAction;
		MovementAction = NewAction;
		OnMovementActionChanged(Prev);
	}
}

void AALSBaseCharacter::SetStance(const EALSStance NewStance, const bool bForce)
{
	if (bForce || Stance != NewStance)
	{
		const EALSStance Prev = Stance;
		Stance = NewStance;
		OnStanceChanged(Prev);
	}
}

void AALSBaseCharacter::SetOverlayOverrideState(const int32 NewState)
{
	OverlayOverrideState = NewState;
}

void AALSBaseCharacter::SetGait(const EALSGait NewGait, const bool bForce)
{
	if (bForce || Gait != NewGait)
	{
		const EALSGait Prev = Gait;
		Gait = NewGait;
		OnGaitChanged(Prev);
	}
}

void AALSBaseCharacter::SetDesiredStance(const EALSStance NewStance)
{
	DesiredStance = NewStance;
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_SetDesiredStance(NewStance);
	}
}

void AALSBaseCharacter::Server_SetDesiredStance_Implementation(const EALSStance NewStance)
{
	SetDesiredStance(NewStance);
}

void AALSBaseCharacter::SetDesiredGait(const EALSGait NewGait)
{
	DesiredGait = NewGait;
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_SetDesiredGait(NewGait);
	}
}

void AALSBaseCharacter::Server_SetDesiredGait_Implementation(const EALSGait NewGait)
{
	SetDesiredGait(NewGait);
}

void AALSBaseCharacter::SetDesiredRotationMode(const EALSRotationMode NewRotMode)
{
	DesiredRotationMode = NewRotMode;
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_SetDesiredRotationMode(NewRotMode);
	}
}

void AALSBaseCharacter::Server_SetDesiredRotationMode_Implementation(const EALSRotationMode NewRotMode)
{
	SetDesiredRotationMode(NewRotMode);
}

void AALSBaseCharacter::SetRotationMode(const EALSRotationMode NewRotationMode, const bool bForce)
{
	if (bForce || RotationMode != NewRotationMode)
	{
		const EALSRotationMode Prev = RotationMode;
		RotationMode = NewRotationMode;
		OnRotationModeChanged(Prev);

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetRotationMode(NewRotationMode, bForce);
		}
	}
}

void AALSBaseCharacter::Server_SetRotationMode_Implementation(const EALSRotationMode NewRotationMode, const bool bForce)
{
	SetRotationMode(NewRotationMode, bForce);
}

void AALSBaseCharacter::SetFlightState(const EALSFlightState NewFlightState, const bool bForce)
{
	if (bForce || FlightState != NewFlightState)
	{
		// If we are trying for a mode other than turning flight off, verify the character is able to fly.
		if (NewFlightState != EALSFlightState::None)
		{
			if (!CanFly()) return;
		}

		const EALSFlightState Prev = FlightState;
		FlightState = NewFlightState;
		OnFlightStateChanged(Prev);

		if (FlightState == EALSFlightState::None) // We want to stop flight.
		{
			// Setting the movement mode to falling is pretty safe. If the character is grounded, than the movement
			// component will know to set it to Walking instead.
			GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		}
		else if (Prev == EALSFlightState::None) // We want to start flight.
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		}
		else // Changing from one flight mode to another logic:
		{
			// Currently blank
		}

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetFlightState(NewFlightState, bForce);
		}
	}
}

void AALSBaseCharacter::Server_SetFlightState_Implementation(const EALSFlightState NewFlightState, const bool bForce)
{
	SetFlightState(NewFlightState);
}

void AALSBaseCharacter::SetViewMode(const EALSViewMode NewViewMode, const bool bForce)
{
	if (bForce || ViewMode != NewViewMode)
	{
		const EALSViewMode Prev = ViewMode;
		ViewMode = NewViewMode;
		OnViewModeChanged(Prev);

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetViewMode(NewViewMode, bForce);
		}
	}
}

void AALSBaseCharacter::Server_SetViewMode_Implementation(const EALSViewMode NewViewMode, const bool bForce)
{
	SetViewMode(NewViewMode, bForce);
}

void AALSBaseCharacter::SetOverlayState(const EALSOverlayState NewState, const bool bForce)
{
	if (bForce || OverlayState != NewState)
	{
		const EALSOverlayState Prev = OverlayState;
		OverlayState = NewState;
		OnOverlayStateChanged(Prev);

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetOverlayState(NewState, bForce);
		}
	}
}

void AALSBaseCharacter::SetGroundedEntryState(const EALSGroundedEntryState NewState)
{
	GroundedEntryState = NewState;
}


void AALSBaseCharacter::Server_SetOverlayState_Implementation(const EALSOverlayState NewState, const bool bForce)
{
	SetOverlayState(NewState, bForce);
}

void AALSBaseCharacter::EventOnLanded()
{
	const float VelZ = FMath::Abs(GetCharacterMovement()->Velocity.Z);

	if (bRagdollOnLand && VelZ > RagdollOnLandVelocity)
	{
		ReplicatedRagdollStart();
	}
	else if (bBreakfallOnLand && ((bHasMovementInput && VelZ >= BreakfallOnLandVelocity) || bBreakFallNextLanding))
	{
		OnBreakfall();
		bBreakFallNextLanding = false;
	}
	else
	{
		GetCharacterMovement()->BrakingFrictionFactor = bHasMovementInput ? 0.5f : 3.0f;

		// After 0.5 secs, reset braking friction factor to zero
		GetWorldTimerManager().SetTimer(OnLandedFrictionResetTimer, this,
		                                &AALSBaseCharacter::OnLandFrictionReset, 0.5f, false);
	}
}

void AALSBaseCharacter::Multicast_OnLanded_Implementation()
{
	if (!IsLocallyControlled())
	{
		EventOnLanded();
	}
}

void AALSBaseCharacter::EventOnJumped()
{
	// Set the new In Air Rotation to the velocity rotation if speed is greater than 100.
	InAirRotation = Speed > 100.0f ? LastVelocityRotation : GetActorRotation();

	OnJumpedDelegate.Broadcast();
}

void AALSBaseCharacter::Server_PlayMontage_Implementation(UAnimMontage* Montage, const float PlayRate)
{
	if (GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);
	}

	ForceNetUpdate();
	Multicast_PlayMontage(Montage, PlayRate);
}

void AALSBaseCharacter::Multicast_PlayMontage_Implementation(UAnimMontage* Montage, const float PlayRate)
{
	if (GetMesh()->GetAnimInstance() && !IsLocallyControlled())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);
	}
}

void AALSBaseCharacter::Multicast_OnJumped_Implementation()
{
	if (!IsLocallyControlled())
	{
		EventOnJumped();
	}
}

void AALSBaseCharacter::Server_RagdollStart_Implementation()
{
	Multicast_RagdollStart();
}

void AALSBaseCharacter::Multicast_RagdollStart_Implementation()
{
	RagdollStart();
}

void AALSBaseCharacter::Server_RagdollEnd_Implementation(const FVector CharacterLocation)
{
	Multicast_RagdollEnd(CharacterLocation);
}

void AALSBaseCharacter::Multicast_RagdollEnd_Implementation(const FVector CharacterLocation)
{
	RagdollEnd();
}

void AALSBaseCharacter::SetActorLocationAndTargetRotation(const FVector NewLocation, const FRotator NewRotation)
{
	SetActorLocationAndRotation(NewLocation, NewRotation);
	TargetRotation = NewRotation;
}

void AALSBaseCharacter::ForceUpdateCharacterState()
{
	SetGait(DesiredGait, true);
	SetStance(DesiredStance, true);
	SetRotationMode(DesiredRotationMode, true);
	SetViewMode(ViewMode, true);
	SetOverlayState(OverlayState, true);
	SetMovementState(MovementState, true);
	SetMovementAction(MovementAction, true);
}

FALSMovementSettings AALSBaseCharacter::GetTargetMovementSettings() const
{
	check(MovementData);

	FALSMovementStanceSettings* StanceSettings = nullptr;

	if		(RotationMode == EALSRotationMode::VelocityDirection)	StanceSettings = &MovementData->VelocityDirection;
	else if (RotationMode == EALSRotationMode::LookingDirection)	StanceSettings = &MovementData->LookingDirection;
	else if (RotationMode == EALSRotationMode::Aiming)				StanceSettings = &MovementData->Aiming;

	FALSMovementSettings Settings;

	if (MovementState == EALSMovementState::Grounded)
	{
		if (Stance == EALSStance::Standing)					Settings = StanceSettings->Standing;
		if (Stance == EALSStance::Crouching)				Settings = StanceSettings->Crouching;
	}
	else if (MovementState == EALSMovementState::Flight)	Settings = StanceSettings->Flying;
	else if (MovementState == EALSMovementState::Swimming)	Settings = StanceSettings->Swimming;
	else													Settings = StanceSettings->Standing; // Default;

	for (auto Modifier : MovementModifiers)
	{
		Modifier.ApplyModifier(Settings, MovementState);
	}

	return Settings;
}

void AALSBaseCharacter::AddMovementModifier(const FALSMovementModifier Modifier)
{
	if (Modifier.IsValid())
	{
		MovementModifiers.AddUnique(Modifier);
		MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
	}
}

void AALSBaseCharacter::RemoveMovementModifier(const FName ModifierID)
{
	MovementModifiers.Remove(ModifierID);
	MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
}

void AALSBaseCharacter::SetMovementModifierTime(const FName ModifierID, const float Time)
{
	if (FALSMovementModifier* Modifier = MovementModifiers.FindByKey(ModifierID))
	{
		Modifier->SetTime(Time);
		MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
	}
}

bool AALSBaseCharacter::CanSprint() const
{
	// Determine if the character is currently able to sprint based on the Rotation mode and current acceleration
	// (input) rotation. If the character is in the Looking Rotation mode, only allow sprinting if there is full
	// movement input and it is faced forward relative to the camera + or - 50 degrees.

	if (!bHasMovementInput || RotationMode == EALSRotationMode::Aiming)
	{
		return false;
	}

	const bool bValidInputAmount = MovementInputAmount > 0.9f;

	if (RotationMode == EALSRotationMode::VelocityDirection)
	{
		return bValidInputAmount;
	}

	if (RotationMode == EALSRotationMode::LookingDirection)
	{
		const FRotator AccRot = ReplicatedCurrentAcceleration.ToOrientationRotator();
		FRotator Delta = AccRot - AimingRotation;
		Delta.Normalize();

		return bValidInputAmount && FMath::Abs(Delta.Yaw) < 50.0f;
	}

	return false;
}

bool AALSBaseCharacter::CanFly() const
{
	return MyCharacterMovementComponent->CanEverFly() && FlightCheck();
}

bool AALSBaseCharacter::FlightCheck_Implementation() const
{
	return true;
	// @todo
	//&& FMath::IsWithin(Temperature, FlightTempBounds.X, FlightTempBounds.Y)
	//&& EffectiveWeight < FlightWeightCutOff;
}

bool AALSBaseCharacter::FlightInterruptCheck_Implementation(UPrimitiveComponent* MyComp, AActor* Other,
                                                            UPrimitiveComponent* OtherComp, bool bSelfMoved,
                                                            FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
                                                            const FHitResult& Hit) const
{
	float MyVelLen;
	FVector MyVelDir;
	GetVelocity().GetAbs().ToDirectionAndLength(MyVelDir, MyVelLen);
	return MyVelLen >= FlightInterruptThreshold;
}

FVector AALSBaseCharacter::GetMovementInput() const
{
	return ReplicatedCurrentAcceleration;
}

float AALSBaseCharacter::GetAnimCurveValue(const FName CurveName) const
{
	if (GetMesh()->GetAnimInstance())
	{
		return GetMesh()->GetAnimInstance()->GetCurveValue(CurveName);
	}

	return 0.0f;
}

void AALSBaseCharacter::SetVisibleMesh(USkeletalMesh* NewVisibleMesh)
{
	if (VisibleMesh != NewVisibleMesh)
	{
		const USkeletalMesh* Prev = VisibleMesh;
		VisibleMesh = NewVisibleMesh;
		OnVisibleMeshChanged(Prev);

		if (GetLocalRole() != ROLE_Authority)
		{
			Server_SetVisibleMesh(NewVisibleMesh);
		}
	}
}

void AALSBaseCharacter::Server_SetVisibleMesh_Implementation(USkeletalMesh* NewVisibleMesh)
{
	SetVisibleMesh(NewVisibleMesh);
}

void AALSBaseCharacter::SetRightShoulder(const bool bNewRightShoulder)
{
	bRightShoulder = bNewRightShoulder;
	if (CameraBehavior)
	{
		CameraBehavior->bRightShoulder = bRightShoulder;
	}
}

ECollisionChannel AALSBaseCharacter::GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius)
{
	TraceOrigin = GetActorLocation();
	TraceRadius = 10.0f;
	return ECC_Visibility;
}

FTransform AALSBaseCharacter::GetThirdPersonPivotTarget()
{
	return GetActorTransform();
}

FVector AALSBaseCharacter::GetFirstPersonCameraTarget()
{
	return GetMesh()->GetSocketLocation(NAME_FP_Camera);
}

void AALSBaseCharacter::GetCameraParameters(float& TPFOVOut, float& FPFOVOut, bool& bRightShoulderOut) const
{
	TPFOVOut = ThirdPersonFOV;
	FPFOVOut = FirstPersonFOV;
	bRightShoulderOut = bRightShoulder;
}

void AALSBaseCharacter::RagdollUpdate(const float DeltaTime)
{
	GetMesh()->bOnlyAllowAutonomousTickPose = false;

	// Set the Last Ragdoll Velocity.
	const FVector NewRagdollVel = GetMesh()->GetPhysicsLinearVelocity(NAME_root);
	LastRagdollVelocity = (NewRagdollVel != FVector::ZeroVector || IsLocallyControlled())
		                      ? NewRagdollVel
		                      : LastRagdollVelocity / 2;

	// Use the Ragdoll Velocity to scale the ragdoll's joint strength for physical animation.
	const float SpringValue = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 1000.0f}, {0.0f, 25000.0f},
	                                                            LastRagdollVelocity.Size());
	GetMesh()->SetAllMotorsAngularDriveParams(SpringValue, 0.0f, 0.0f, false);

	// Disable Gravity if falling faster than -4000 to prevent continual acceleration.
	// This also prevents the ragdoll from going through the floor.
	const bool bEnableGrav = LastRagdollVelocity.Z > -4000.0f;
	GetMesh()->SetEnableGravity(bEnableGrav);

	// Update the Actor location to follow the ragdoll.
	SetActorLocationDuringRagdoll(DeltaTime);
}

void AALSBaseCharacter::SetActorLocationDuringRagdoll(float DeltaTime)
{
	if (IsLocallyControlled())
	{
		// Set the pelvis as the target location.
		TargetRagdollLocation = GetMesh()->GetSocketLocation(NAME_Pelvis);
		if (!HasAuthority())
		{
			Server_SetMeshLocationDuringRagdoll(TargetRagdollLocation);
		}
	}

	// Determine whether the ragdoll is facing up or down and set the target rotation accordingly.
	const FRotator PelvisRot = GetMesh()->GetSocketRotation(NAME_Pelvis);

	if (bReversedPelvis)
	{
		bRagdollFaceUp = PelvisRot.Roll > 0.0f;
	}
	else
	{
		bRagdollFaceUp = PelvisRot.Roll < 0.0f;
	}


	const FRotator TargetRagdollRotation(0.0f, bRagdollFaceUp ? PelvisRot.Yaw - 180.0f : PelvisRot.Yaw, 0.0f);

	// Trace downward from the target location to offset the target location,
	// preventing the lower half of the capsule from going through the floor when the ragdoll is laying on the ground.
	const FVector TraceVect(TargetRagdollLocation.X, TargetRagdollLocation.Y,
	                        TargetRagdollLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	UWorld* World = GetWorld();
	check(World);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(HitResult, TargetRagdollLocation, TraceVect,
	                                                  ECC_Visibility, Params);

	if (ALSDebugComponent && ALSDebugComponent->GetShowTraces())
	{
		UALSDebugComponent::DrawDebugLineTraceSingle(World,
		                                             TargetRagdollLocation,
		                                             TraceVect,
		                                             EDrawDebugTrace::Type::ForOneFrame,
		                                             bHit,
		                                             HitResult,
		                                             FLinearColor::Red,
		                                             FLinearColor::Green,
		                                             1.0f);
	}

	bRagdollOnGround = HitResult.IsValidBlockingHit();
	FVector NewRagdollLoc = TargetRagdollLocation;

	if (bRagdollOnGround)
	{
		const float ImpactDistZ = FMath::Abs(HitResult.ImpactPoint.Z - HitResult.TraceStart.Z);
		NewRagdollLoc.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - ImpactDistZ + 2.0f;
	}
	if (!IsLocallyControlled())
	{
		ServerRagdollPull = FMath::FInterpTo(ServerRagdollPull, 750.0f, DeltaTime, 0.6f);
		float RagdollSpeed = FVector(LastRagdollVelocity.X, LastRagdollVelocity.Y, 0).Size();
		FName RagdollSocketPullName = RagdollSpeed > 300 ? NAME_spine_03 : NAME_pelvis;
		GetMesh()->AddForce(
			(TargetRagdollLocation - GetMesh()->GetSocketLocation(RagdollSocketPullName)) * ServerRagdollPull,
			RagdollSocketPullName, true);
	}
	SetActorLocationAndTargetRotation(bRagdollOnGround ? NewRagdollLoc : TargetRagdollLocation, TargetRagdollRotation);
}

void AALSBaseCharacter::OnMovementModeChanged(const EMovementMode PrevMovementMode, const uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	// Use the Character Movement Mode changes to set the Movement States to the right values. This allows you to have
	// a custom set of movement states but still use the functionality of the default character movement component.
	switch (GetCharacterMovement()->MovementMode)
	{
	case MOVE_None: SetMovementState(EALSMovementState::None); break;
	case MOVE_Walking: SetMovementState(EALSMovementState::Grounded); break;
	case MOVE_NavWalking: SetMovementState(EALSMovementState::Grounded); break;
	case MOVE_Falling: SetMovementState(EALSMovementState::Freefall); break;
	case MOVE_Swimming: SetMovementState(EALSMovementState::Swimming); break;
	case MOVE_Flying: SetMovementState(EALSMovementState::Flight); break;
	case MOVE_Custom: SetMovementState(EALSMovementState::None); break;
	case MOVE_MAX: SetMovementState(EALSMovementState::None); break;
	default: SetMovementState(EALSMovementState::None); break;
	}
}

void AALSBaseCharacter::OnMovementStateChanged(const EALSMovementState PreviousState)
{
	if (MovementState != EALSMovementState::Flight)
	{
		SetFlightState(EALSFlightState::None);
	}

	if (MovementState == EALSMovementState::Freefall)
	{
		if (MovementAction == EALSMovementAction::None)
		{
			// If the character enters the air, set the In Air Rotation and uncrouch if crouched.
			InAirRotation = GetActorRotation();
			if (Stance == EALSStance::Crouching)
			{
				UnCrouch();
			}
		}
		else if (MovementAction == EALSMovementAction::Rolling && bRagdollOnRollfall)
		{
			// If the character is currently rolling, enable the ragdoll.
			ReplicatedRagdollStart();
		}
	}

	if (CameraBehavior)
	{
		CameraBehavior->MovementState = MovementState;
	}
}

void AALSBaseCharacter::OnMovementActionChanged(const EALSMovementAction PreviousAction)
{
	// Make the character crouch if performing a roll.
	if (MovementAction == EALSMovementAction::Rolling)
	{
		Crouch();
	}

	if (PreviousAction == EALSMovementAction::Rolling)
	{
		if (DesiredStance == EALSStance::Standing)
		{
			UnCrouch();
		}
		else if (DesiredStance == EALSStance::Crouching)
		{
			Crouch();
		}
	}

	if (CameraBehavior)
	{
		CameraBehavior->MovementAction = MovementAction;
	}
}

void AALSBaseCharacter::OnStanceChanged(const EALSStance PreviousStance)
{
	if (CameraBehavior)
	{
		CameraBehavior->Stance = Stance;
	}

	MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
}

void AALSBaseCharacter::OnRotationModeChanged(EALSRotationMode PreviousRotationMode)
{
	if (RotationMode == EALSRotationMode::VelocityDirection && ViewMode == EALSViewMode::FirstPerson)
	{
		// If the new rotation mode is Velocity Direction and the character is in First Person,
		// set the ViewMode to Third Person.
		SetViewMode(EALSViewMode::ThirdPerson);
	}

	if (CameraBehavior)
	{
		CameraBehavior->SetRotationMode(RotationMode);
	}

	MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
}

void AALSBaseCharacter::OnFlightStateChanged(EALSFlightState PreviousFlightState)
{
	if (CameraBehavior)
	{
		CameraBehavior->FlightState = FlightState;
	}
}

void AALSBaseCharacter::OnGaitChanged(const EALSGait PreviousGait)
{
	if (CameraBehavior)
	{
		CameraBehavior->Gait = Gait;
	}
}

void AALSBaseCharacter::OnViewModeChanged(const EALSViewMode PreviousViewMode)
{
	if (ViewMode == EALSViewMode::ThirdPerson)
	{
		if (RotationMode == EALSRotationMode::VelocityDirection || RotationMode == EALSRotationMode::LookingDirection)
		{
			// If Third Person, set the rotation mode back to the desired mode.
			SetRotationMode(DesiredRotationMode);
		}
	}
	else if (ViewMode == EALSViewMode::FirstPerson && RotationMode == EALSRotationMode::VelocityDirection)
	{
		// If First Person, set the rotation mode to looking direction if currently in the velocity direction mode.
		SetRotationMode(EALSRotationMode::LookingDirection);
	}

	// @todo why do we need both a virtual handler and a delegate. cannot just one do?
	K2_OnViewModeChanged(PreviousViewMode);
	ViewModeChangedDelegate.Broadcast(this, PreviousViewMode);

	if (CameraBehavior)
	{
		CameraBehavior->ViewMode = ViewMode;
	}
}

void AALSBaseCharacter::OnOverlayStateChanged(const EALSOverlayState PreviousState)
{
}

void AALSBaseCharacter::OnVisibleMeshChanged(const USkeletalMesh* PreviousSkeletalMesh)
{
	// Update the Skeletal Mesh before we update materials and anim bp variables
	GetMesh()->SetSkeletalMesh(VisibleMesh);

	// Reset materials to their new mesh defaults
	if (GetMesh() != nullptr)
	{
		for (int32 MaterialIndex = 0; MaterialIndex < GetMesh()->GetNumMaterials(); ++MaterialIndex)
		{
			GetMesh()->SetMaterial(MaterialIndex, nullptr);
		}
	}

	// Force set variables. This ensures anim instance & character stay synchronized on mesh changes
	ForceUpdateCharacterState();
}

void AALSBaseCharacter::OnStartCrouch(const float HalfHeightAdjust, const float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	SetStance(EALSStance::Crouching);
}

void AALSBaseCharacter::OnEndCrouch(const float HalfHeightAdjust, const float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	SetStance(EALSStance::Standing);
}

void AALSBaseCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	if (IsLocallyControlled())
	{
		EventOnJumped();
	}
	if (HasAuthority())
	{
		Multicast_OnJumped();
	}
}

void AALSBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (IsLocallyControlled())
	{
		EventOnLanded();
	}
	if (HasAuthority())
	{
		Multicast_OnLanded();
	}
}

void AALSBaseCharacter::OnLandFrictionReset()
{
	// Reset the braking friction
	GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
}

void AALSBaseCharacter::SetEssentialValues(const float DeltaTime)
{
	if (GetLocalRole() != ROLE_SimulatedProxy)
	{
		ReplicatedCurrentAcceleration = GetCharacterMovement()->GetCurrentAcceleration();
		ReplicatedControlRotation = GetControlRotation();
		EasedMaxAcceleration = GetCharacterMovement()->GetMaxAcceleration();
	}
	else
	{
		EasedMaxAcceleration = GetCharacterMovement()->GetMaxAcceleration() != 0
			                       ? GetCharacterMovement()->GetMaxAcceleration()
			                       : EasedMaxAcceleration / 2;
	}

	// Interp AimingRotation to current control rotation for smooth character rotation movement. Decrease InterpSpeed
	// for slower but smoother movement.
	AimingRotation = FMath::RInterpTo(AimingRotation, ReplicatedControlRotation, DeltaTime, 30);

	// These values represent how the capsule is moving as well as how it wants to move, and therefore are essential
	// for any data driven animation system. They are also used throughout the system for various functions,
	// so I found it is easiest to manage them all in one place.

	const FVector CurrentVel = GetVelocity();

	// Set the amount of Acceleration.
	const FVector NewAcceleration = (CurrentVel - PreviousVelocity) / DeltaTime;
	Acceleration = NewAcceleration.IsNearlyZero() || IsLocallyControlled() ? NewAcceleration : Acceleration / 2;

	// Determine if the character is moving by getting it's speed. The Speed equals the length of the horizontal (x y)
	// velocity, so it does not take vertical movement into account. If the character is moving, update the last
	// velocity rotation. This value is saved because it might be useful to know the last orientation of movement
	// even after the character has stopped.
	Speed = CurrentVel.Size2D();
	bIsMoving = Speed > 1.0f;
	if (bIsMoving)
	{
		LastVelocityRotation = CurrentVel.ToOrientationRotator();
	}

	// Determine if the character has movement input by getting its movement input amount.
	// The Movement Input Amount is equal to the current acceleration divided by the max acceleration so that
	// it has a range of 0-1, 1 being the maximum possible amount of input, and 0 being none.
	// If the character has movement input, update the Last Movement Input Rotation.
	MovementInputAmount = ReplicatedCurrentAcceleration.Size() / EasedMaxAcceleration;
	bHasMovementInput = MovementInputAmount > 0.0f;
	if (bHasMovementInput)
	{
		LastMovementInputRotation = ReplicatedCurrentAcceleration.ToOrientationRotator();
	}

	// Set the Aim Yaw rate by comparing the current and previous Aim Yaw value, divided by Delta Seconds.
	// This represents the speed the camera is rotating left to right.
	AimYawRate = FMath::Abs((AimingRotation.Yaw - PreviousAimYaw) / DeltaTime);
}

void AALSBaseCharacter::UpdateCharacterMovement()
{
	// Set the Allowed Gait
	const EALSGait AllowedGait = GetAllowedGait();

	// Determine the Actual Gait. If it is different from the current Gait, Set the new Gait Event.
	const EALSGait ActualGait = GetActualGait(AllowedGait);

	if (ActualGait != Gait)
	{
		SetGait(ActualGait);
	}

	// Update the Character Max Walk Speed to the configured speeds based on the currently Allowed Gait.
	MyCharacterMovementComponent->SetAllowedGait(AllowedGait);
}

void AALSBaseCharacter::UpdateFlightMovement(const float DeltaTime)
{
	if (AlwaysCheckFlightConditions)
	{
		if (!CanFly())
		{
			SetFlightState(EALSFlightState::None);
			return;
		}
	}

	// The rest of this function calculates the auto-hover strength. This is how much downward force is generated by
	// the wings to keep the character afloat.
	float AutoHover;

	// Represents the strength of the wings forcing downward when flying, measured in the units below the character that
	// the pressure gradient extends.
	const float WingPressureDepth = 200; //@todo old calculation: FlightStrengthPassive / EffectiveWeight;

	FVector VelocityDirection;
	float VelocityLength;
	GetVelocity().ToDirectionAndLength(VelocityDirection, VelocityLength);

	const float VelocityAlpha = FMath::GetMappedRangeValueClamped(FVector2f{0, GetCharacterMovement()->MaxFlySpeed * 1.5f},
																  FVector2f{0, 1},
																  VelocityLength);

	const FVector PressureDirection = FMath::Lerp(FVector(0, 0, -1), -VelocityDirection, VelocityAlpha);

	const float PressureAlpha = FlightDistanceCheck(WingPressureDepth, PressureDirection) / WingPressureDepth;

	// If a pressure curve is used, modify speed. Otherwise, default to 1 for no effect.
	float GroundPressure;
	//if (GroundPressureFalloff)
	//{
	//	GroundPressure = GroundPressureFalloff->GetFloatValue(PressureAlpha);
	//}
	//else
	{
		GroundPressure = 1;
	}

	const float LocalTemperatureAffect = 1; // TemperatureAffect.Y;
	const float LocalWeightAffect = 1; //WeightAffect.Y;

	// @TODO Design an algorithm for calculating thrust, and use it to determine lift. modify auto-thrust with that so that the player slowly drifts down when too heavy.

	switch (FlightState)
	{
	case EALSFlightState::None: return;
	case EALSFlightState::Hovering:
		AutoHover = (GroundPressure + 0.5) / 1.5 * LocalTemperatureAffect * (LocalWeightAffect / 2);
		break;
	case EALSFlightState::Aerial:
		AutoHover = (GroundPressure + 0.5) / 1.5 * LocalTemperatureAffect * LocalWeightAffect;
		break;
	//case EALSFlightState::Raising:
	//	AutoHover = (GroundPressure + FlightStrengthActive) * LocalTemperatureAffect * (
	//		LocalWeightAffect * 1.5);
	//	break;
	//case EALSFlightState::Lowering:
	//	AutoHover = (GroundPressure * 0.5f) + (-FlightStrengthActive + (LocalTemperatureAffect
	//		- 1)) + -LocalWeightAffect;
	//	break;
	default: return;
	}

	const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
	//AddMovementInput(UKismetMathLibrary::GetUpVector(DirRotator), AutoHover, true);
}

void AALSBaseCharacter::UpdateGroundedRotation(const float DeltaTime)
{
	if (MovementAction == EALSMovementAction::None)
	{
		const bool bCanUpdateMovingRot = ((bIsMoving && bHasMovementInput) || Speed > 150.0f) && !HasAnyRootMotion();
		if (bCanUpdateMovingRot)
		{
			const double GroundedRotationRate = CalculateGroundedRotationRate();
			if (RotationMode == EALSRotationMode::VelocityDirection)
			{
				// Velocity Direction Rotation
				SmoothCharacterRotation({0.0f, LastVelocityRotation.Yaw, 0.0f}, 800.0f, GroundedRotationRate,
				                        DeltaTime);
			}
			else if (RotationMode == EALSRotationMode::LookingDirection)
			{
				// Looking Direction Rotation
				double YawValue;
				if (Gait == EALSGait::Sprinting)
				{
					YawValue = LastVelocityRotation.Yaw;
				}
				else
				{
					// Walking or Running..
					const float YawOffsetCurveVal = GetAnimCurveValue(NAME_YawOffset);
					YawValue = AimingRotation.Yaw + YawOffsetCurveVal;
				}
				SmoothCharacterRotation({0.0f, YawValue, 0.0f}, 500.0f, GroundedRotationRate, DeltaTime);
			}
			else if (RotationMode == EALSRotationMode::Aiming)
			{
				const float ControlYaw = AimingRotation.Yaw;
				SmoothCharacterRotation({0.0f, ControlYaw, 0.0f}, 1000.0f, 20.0f, DeltaTime);
			}
		}
		else
		{
			// Not Moving

			if ((ViewMode == EALSViewMode::ThirdPerson && RotationMode == EALSRotationMode::Aiming) ||
				ViewMode == EALSViewMode::FirstPerson)
			{
				LimitRotation(-100.0f, 100.0f, 20.0f, DeltaTime);
			}

			// Apply the RotationAmount curve from Turn In Place Animations.
			// The Rotation Amount curve defines how much rotation should be applied each frame,
			// and is calculated for animations that are animated at 30fps.

			const float RotAmountCurve = GetAnimCurveValue(NAME_RotationAmount);

			if (FMath::Abs(RotAmountCurve) > 0.001f)
			{
				if (GetLocalRole() == ROLE_AutonomousProxy)
				{
					TargetRotation.Yaw = UKismetMathLibrary::NormalizeAxis(
						TargetRotation.Yaw + (RotAmountCurve * (DeltaTime / (1.0f / 30.0f))));
					SetActorRotation(TargetRotation);
				}
				else
				{
					AddActorWorldRotation({0, RotAmountCurve * (DeltaTime / (1.0f / 30.0f)), 0});
				}
				TargetRotation = GetActorRotation();
			}
		}
	}
	else if (MovementAction == EALSMovementAction::Rolling)
	{
		// Rolling Rotation (Not allowed on networked games)
		if (!bEnableNetworkOptimizations && bHasMovementInput)
		{
			SmoothCharacterRotation({0.0f, LastMovementInputRotation.Yaw, 0.0f}, 0.0f, 2.0f, DeltaTime);
		}
	}

	// Other actions are ignored...
}

void AALSBaseCharacter::UpdateFallingRotation(const float DeltaTime)
{
	if (RotationMode == EALSRotationMode::VelocityDirection || RotationMode == EALSRotationMode::LookingDirection)
	{
		// Velocity / Looking Direction Rotation
		SmoothCharacterRotation({0.0f, InAirRotation.Yaw, 0.0f}, 0.0f, 5.0f, DeltaTime);
	}
	else if (RotationMode == EALSRotationMode::Aiming)
	{
		// Aiming Rotation
		SmoothCharacterRotation({0.0f, AimingRotation.Yaw, 0.0f}, 0.0f, 15.0f, DeltaTime);
		InAirRotation = GetActorRotation();
	}
}

void AALSBaseCharacter::UpdateFlightRotation(const float DeltaTime)
{
	const float SpeedCache = GetMyMovementComponent()->GetMappedSpeed();
	const float CheckAltitude = SpeedCache * 100.f;

	const float FlightRotationRate = CalculateFlightRotationRate();

	// Map distance to ground to a unit scalar.
	const float Alpha_Altitude = FMath::GetMappedRangeValueClamped(FVector2f{0.f, CheckAltitude}, FVector2f{0.f, 1.f}, RelativeAltitude);

	// Combine unit scalars equal to smaller.
	const float RotationAlpha = Alpha_Altitude * (SpeedCache / 3);

	// Calculate input leaning.
	const FVector Lean = GetActorRotation().UnrotateVector(GetMovementInput().GetSafeNormal()) * MaxFlightLean * RotationAlpha;

	const float Pitch = FMath::FInterpTo(GetActorRotation().Pitch, Lean.X * -1, DeltaTime, FlightRotationRate);
	const float Roll = FMath::FInterpTo(GetActorRotation().Roll, Lean.Y, DeltaTime, FlightRotationRate);

	const bool bCanUpdateMovingRot = ((bIsMoving && bHasMovementInput) || Speed > 150.0f) && !HasAnyRootMotion();
	if (bCanUpdateMovingRot)
	{
		if (RotationMode == EALSRotationMode::VelocityDirection)
		{
			// Velocity Rotation
			const float InterpSpeed = FMath::GetMappedRangeValueClamped<float, float>({0.f, 3.f}, {0.1f, FlightRotationRate}, SpeedCache);
			SmoothCharacterRotation({Pitch, LastVelocityRotation.Yaw, Roll}, 100.0f, InterpSpeed, DeltaTime);
		}
		else if (RotationMode == EALSRotationMode::LookingDirection)
		{
			// Looking Direction Rotation
			float YawValue;
			if (Gait == EALSGait::Sprinting)
			{
				YawValue = LastVelocityRotation.Yaw;
			}
			else
			{
				// Walking or Running..
				//const float YawOffsetCurveVal = MainAnimInstance->GetCurveValue(NAME_YawOffset);
				YawValue = AimingRotation.Yaw; //+ YawOffsetCurveVal;
			}
			SmoothCharacterRotation({Pitch, YawValue, Roll}, 100.0f, FlightRotationRate, DeltaTime);
		}
		else if (RotationMode == EALSRotationMode::Aiming)
		{
			// Aiming Rotation and looking direction
			const float FinalRoll = Roll + AimingRotation.Roll / 2;
			SmoothCharacterRotation({Pitch, AimingRotation.Yaw, FinalRoll}, 500.0f, FlightRotationRate, DeltaTime);
		}
	}
	else
	{
			SmoothCharacterRotation({0, GetActorRotation().Yaw, 0}, 500.0f, FlightRotationRate, DeltaTime);
	}

	InAirRotation = GetActorRotation();
}

void AALSBaseCharacter::UpdateSwimmingRotation(const float DeltaTime)
{
	//@todo this is a visual effect that should probably be handled by animations, not code.
	float const Lean = FMath::GetMappedRangeValueUnclamped(FVector2f{0, 3}, FVector2f{0, 90}, GetMyMovementComponent()->GetMappedSpeed());

	SmoothCharacterRotation({Lean * -Acceleration.X, AimingRotation.Yaw, 0.0f}, 0.f, 2.5f, DeltaTime);
}

float AALSBaseCharacter::FlightDistanceCheck(float CheckDistance, FVector Direction) const
{
	UWorld* World = GetWorld();
	if (!World) return 0.f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const FVector CheckStart = GetActorLocation() - FVector{0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()};
	const FVector CheckEnd = CheckStart + (Direction * CheckDistance);
	World->LineTraceSingleByChannel(HitResult, CheckStart, CheckEnd, UALS_Settings::Get()->FlightCheckChannel, Params);

#if WITH_EDITOR
	//if (DrawDebug) DrawDebugLine(World, CheckStart, CheckEnd, FColor::Silver, false, 0.5f, 0, 2);
#endif

	if (HitResult.bBlockingHit) { return HitResult.Distance; }
	return CheckDistance;
}

EALSGait AALSBaseCharacter::GetAllowedGait() const
{
	// Calculate the Allowed Gait. This represents the maximum Gait the character is currently allowed to be in,
	// and can be determined by the desired gait, the rotation mode, the stance, etc. For example,
	// if you wanted to force the character into a walking state while indoors, this could be done here.

	if (Stance == EALSStance::Standing)
	{
		if (RotationMode != EALSRotationMode::Aiming)
		{
			if (DesiredGait == EALSGait::Sprinting)
			{
				return CanSprint() ? EALSGait::Sprinting : EALSGait::Running;
			}
			return DesiredGait;
		}
	}

	// Crouching stance & Aiming rot mode has same behaviour

	if (DesiredGait == EALSGait::Sprinting)
	{
		return EALSGait::Running;
	}

	return DesiredGait;
}

EALSGait AALSBaseCharacter::GetActualGait(const EALSGait AllowedGait) const
{
	// Get the Actual Gait. This is calculated by the actual movement of the character,  and so it can be different
	// from the desired gait or allowed gait. For instance, if the Allowed Gait becomes walking,
	// the Actual gait will still be running until the character decelerates to the walking speed.

	const float LocWalkSpeed = MyCharacterMovementComponent->CurrentMovementSettings.WalkSpeed;
	const float LocRunSpeed = MyCharacterMovementComponent->CurrentMovementSettings.RunSpeed;

	if (Speed > LocRunSpeed + 10.0f)
	{
		if (AllowedGait == EALSGait::Sprinting)
		{
			return EALSGait::Sprinting;
		}
		return EALSGait::Running;
	}

	if (Speed >= LocWalkSpeed + 10.0f)
	{
		return EALSGait::Running;
	}

	return EALSGait::Walking;
}

void AALSBaseCharacter::SmoothCharacterRotation(const FRotator Target, const float TargetInterpSpeed,
												const float ActorInterpSpeed, const float DeltaTime)
{
	// Interpolate the Target Rotation for extra smooth rotation behavior
	TargetRotation =
		FMath::RInterpConstantTo(TargetRotation, Target, DeltaTime, TargetInterpSpeed);
	SetActorRotation(
		FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, ActorInterpSpeed));
}

// @todo since much of this comes from the movement comp, consider moving entire function there
float AALSBaseCharacter::CalculateGroundedRotationRate() const
{
	// Calculate the rotation rate by using the current Rotation Rate Curve in the Movement Settings.
	// Using the curve in conjunction with the mapped speed gives you a high level of control over the rotation
	// rates for each speed. Increase the speed if the camera is rotating quickly for more responsive rotation.

	const float MappedSpeedVal = MyCharacterMovementComponent->GetMappedSpeed();
	const float CurveVal =
		MyCharacterMovementComponent->CurrentMovementSettings.RotationRateCurve->GetFloatValue(MappedSpeedVal);
	const float ClampedAimYawRate = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 300.0f}, {1.0f, 3.0f}, AimYawRate);
	return CurveVal * ClampedAimYawRate;
}

float AALSBaseCharacter::CalculateFlightRotationRate() const
{
	// Calculate the rotation rate by using the current Rotation Rate Curve in the Movement Settings.
	// Using the curve in conjunction with the mapped speed gives you a high level of control over the rotation
	// rates for each speed. Increase the speed if the camera is rotating quickly for more responsive rotation.

	const float MappedSpeedVal = MyCharacterMovementComponent->GetMappedSpeed();
	const float CurveVal =
		MyCharacterMovementComponent->CurrentMovementSettings.RotationRateCurve->GetFloatValue(MappedSpeedVal);
	const float ClampedAimYawRate = FMath::GetMappedRangeValueClamped(FVector2f{0.0f, 300.0f}, FVector2f{1.0f, 3.0f}, AimYawRate);
	return CurveVal * ClampedAimYawRate;
}

void AALSBaseCharacter::UpdateRelativeAltitude()
{
	RelativeAltitude = FlightDistanceCheck(TroposphereHeight, FVector::DownVector);
}
void AALSBaseCharacter::LimitRotation(const float AimYawMin, const float AimYawMax, const float InterpSpeed, const float DeltaTime)
{
	// Prevent the character from rotating past a certain angle.
	FRotator Delta = AimingRotation - GetActorRotation();
	Delta.Normalize();
	const float RangeVal = Delta.Yaw;

	if (RangeVal < AimYawMin || RangeVal > AimYawMax)
	{
		const float ControlRotYaw = AimingRotation.Yaw;
		const float TargetYaw = ControlRotYaw + (RangeVal > 0.0f ? AimYawMin : AimYawMax);
		SmoothCharacterRotation({0.0f, TargetYaw, 0.0f}, 0.0f, InterpSpeed, DeltaTime);
	}
}

void AALSBaseCharacter::ForwardMovementAction_Implementation(const float Value)
{
	if (MovementState == EALSMovementState::None || MovementState == EALSMovementState::Ragdoll)
	{
		return;
	}

	// Default camera relative movement behavior
	const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
	AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotator), Value);
}

void AALSBaseCharacter::RightMovementAction_Implementation(const float Value)
{
	if (MovementState == EALSMovementState::None || MovementState == EALSMovementState::Ragdoll)
	{
		return;
	}

	// Default camera relative movement behavior
	const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
	AddMovementInput(UKismetMathLibrary::GetRightVector(DirRotator), Value);
}

void AALSBaseCharacter::UpMovementAction_Implementation(const float Value)
{
	if (MovementState == EALSMovementState::None || MovementState == EALSMovementState::Ragdoll)
	{
		return;
	}

	// Default camera relative movement behavior
	const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
	AddMovementInput(UKismetMathLibrary::GetUpVector(DirRotator), Value);
}

void AALSBaseCharacter::CameraUpAction_Implementation(const float Value)
{
	AddControllerPitchInput(LookUpDownRate * Value);
}

void AALSBaseCharacter::CameraRightAction_Implementation(const float Value)
{
	AddControllerYawInput(LookLeftRightRate * Value);
}

void AALSBaseCharacter::JumpAction_Implementation(const bool bValue)
{
	if (bValue)
	{
		// Jump Action: Press "Jump Action" to end the ragdoll if ragdolling, stand up if crouching, or jump if standing.
		if (JumpPressedDelegate.IsBound())
		{
			JumpPressedDelegate.Broadcast();
		}

		if (MovementAction == EALSMovementAction::None)
		{
			if (MovementState == EALSMovementState::Grounded)
			{
				if (Stance == EALSStance::Standing)
				{
					Jump();
				}
				else if (Stance == EALSStance::Crouching)
				{
					UnCrouch();
				}
			}
			else if (MovementState == EALSMovementState::Ragdoll)
			{
				ReplicatedRagdollEnd();
			}
		}
	}
	else
	{
		StopJumping();
	}
}

void AALSBaseCharacter::SprintAction_Implementation(const bool bValue)
{
	if (bValue)
	{
		SetDesiredGait(EALSGait::Sprinting);
	}
	else
	{
		SetDesiredGait(EALSGait::Running);
	}
}

void AALSBaseCharacter::AimAction_Implementation(const bool bValue)
{
	if (bValue)
	{
		// AimAction: Hold "AimAction" to enter the aiming mode, release to revert back the desired rotation mode.
		SetRotationMode(EALSRotationMode::Aiming);
	}
	else
	{
		if (ViewMode == EALSViewMode::ThirdPerson)
		{
			SetRotationMode(DesiredRotationMode);
		}
		else if (ViewMode == EALSViewMode::FirstPerson)
		{
			SetRotationMode(EALSRotationMode::LookingDirection);
		}
	}
}

void AALSBaseCharacter::CameraTapAction_Implementation()
{
	if (ViewMode == EALSViewMode::FirstPerson)
	{
		// Don't swap shoulders on first person mode
		return;
	}

	// Switch shoulders
	SetRightShoulder(!bRightShoulder);
}

void AALSBaseCharacter::CameraHeldAction_Implementation()
{
	// Switch camera mode
	if (ViewMode == EALSViewMode::FirstPerson)
	{
		SetViewMode(EALSViewMode::ThirdPerson);
	}
	else if (ViewMode == EALSViewMode::ThirdPerson)
	{
		SetViewMode(EALSViewMode::FirstPerson);
	}
}

void AALSBaseCharacter::StanceAction_Implementation()
{
	// Stance Action: Press "Stance Action" to toggle Standing / Crouching, double tap to Roll.

	if (MovementAction != EALSMovementAction::None)
	{
		return;
	}

	UWorld* World = GetWorld();
	check(World);

	const float PrevStanceInputTime = LastStanceInputTime;
	LastStanceInputTime = World->GetTimeSeconds();

	if (LastStanceInputTime - PrevStanceInputTime <= RollDoubleTapTimeout)
	{
		// Roll
		Replicated_PlayMontage(GetRollAnimation(), 1.15f);

		if (Stance == EALSStance::Standing)
		{
			SetDesiredStance(EALSStance::Crouching);
		}
		else if (Stance == EALSStance::Crouching)
		{
			SetDesiredStance(EALSStance::Standing);
		}
		return;
	}

	if (MovementState == EALSMovementState::Grounded)
	{
		if (Stance == EALSStance::Standing)
		{
			SetDesiredStance(EALSStance::Crouching);
			Crouch();
		}
		else if (Stance == EALSStance::Crouching)
		{
			SetDesiredStance(EALSStance::Standing);
			UnCrouch();
		}
	}

	// Notice: MovementState == EALSMovementState::InAir case is removed
}

void AALSBaseCharacter::WalkAction_Implementation()
{
	if (DesiredGait == EALSGait::Walking)
	{
		SetDesiredGait(EALSGait::Running);
	}
	else if (DesiredGait == EALSGait::Running)
	{
		SetDesiredGait(EALSGait::Walking);
	}
}

void AALSBaseCharacter::RagdollAction_Implementation()
{
	// Ragdoll Action: Press "Ragdoll Action" to toggle the ragdoll state on or off.

	if (GetMovementState() == EALSMovementState::Ragdoll)
	{
		ReplicatedRagdollEnd();
	}
	else
	{
		ReplicatedRagdollStart();
	}
}

void AALSBaseCharacter::VelocityDirectionAction_Implementation()
{
	// Select Rotation Mode: Switch the desired (default) rotation mode to Velocity or Looking Direction.
	// This will be the mode the character reverts back to when un-aiming
	SetDesiredRotationMode(EALSRotationMode::VelocityDirection);
	SetRotationMode(EALSRotationMode::VelocityDirection);
}

void AALSBaseCharacter::LookingDirectionAction_Implementation()
{
	SetDesiredRotationMode(EALSRotationMode::LookingDirection);
	SetRotationMode(EALSRotationMode::LookingDirection);
}

void AALSBaseCharacter::ReplicatedRagdollStart()
{
	if (HasAuthority())
	{
		Multicast_RagdollStart();
	}
	else
	{
		Server_RagdollStart();
	}
}

void AALSBaseCharacter::ReplicatedRagdollEnd()
{
	if (HasAuthority())
	{
		Multicast_RagdollEnd(GetActorLocation());
	}
	else
	{
		Server_RagdollEnd(GetActorLocation());
	}
}

void AALSBaseCharacter::OnRep_RotationMode(const EALSRotationMode PrevRotMode)
{
	OnRotationModeChanged(PrevRotMode);
}

void AALSBaseCharacter::OnRep_FlightState(const EALSFlightState PrevFlightState)
{
	OnFlightStateChanged(PrevFlightState);
}

void AALSBaseCharacter::OnRep_ViewMode(const EALSViewMode PrevViewMode)
{
	OnViewModeChanged(PrevViewMode);
}

void AALSBaseCharacter::OnRep_OverlayState(const EALSOverlayState PrevOverlayState)
{
	OnOverlayStateChanged(PrevOverlayState);
}

void AALSBaseCharacter::OnRep_VisibleMesh(USkeletalMesh* NewVisibleMesh)
{
	OnVisibleMeshChanged(NewVisibleMesh);
}