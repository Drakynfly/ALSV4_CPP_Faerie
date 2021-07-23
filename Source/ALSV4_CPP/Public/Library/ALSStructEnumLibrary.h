// Project:         Advanced Locomotion System V4 on C++
// Copyright:       Copyright (C) 2021 Doğa Can Yanıkoğlu
// License:         MIT License (http://www.opensource.org/licenses/mit-license.php)
// Source Code:     https://github.com/dyanikoglu/ALSV4_CPP
// Original Author: Jens Bjarne Myhre
// Contributors: Doğa Can Yanıkoğlu

// ReSharper disable CppUE4CodingStandardNamingViolationWarning
// ReSharper disable CppNonExplicitConvertingConstructor
// ReSharper disable CppNonExplicitConversionOperator

#pragma once

#include "CoreMinimal.h"
#include "ALSCharacterEnumLibrary.h"

#include "ALSStructEnumLibrary.generated.h"


USTRUCT(BlueprintType)
struct FALSMovementState
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	EALSMovementState State = EALSMovementState::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool None_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Grounded_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Freefall_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Flight_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Swimming_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Mantling_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Ragdoll_ = false;

public:
	FALSMovementState()
	{
	}

	FALSMovementState(const EALSMovementState InitialState) { *this = InitialState; }

	const bool& None() const { return None_; }
	const bool& Grounded() const { return Grounded_; }
	const bool& Freefall() const { return Freefall_; }
	const bool& Flight() const { return Flight_; }
	const bool& Swimming() const { return Swimming_; }
	const bool& Mantling() const { return Mantling_; }
	const bool& Ragdoll() const { return Ragdoll_; }

	operator EALSMovementState() const { return State; }

	void operator=(const EALSMovementState NewState)
	{
		State = NewState;
		None_ = State == EALSMovementState::None;
		Grounded_ = State == EALSMovementState::Grounded;
		Freefall_ = State == EALSMovementState::Freefall;
		Swimming_ = State == EALSMovementState::Swimming;
		Flight_ = State == EALSMovementState::Flight;
		Mantling_ = State == EALSMovementState::Mantling;
		Ragdoll_ = State == EALSMovementState::Ragdoll;
	}
};

USTRUCT(BlueprintType)
struct FALSFlightMode
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Flight Mode")
	EALSFlightMode FlightMode = EALSFlightMode::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Flight Mode")
	bool None_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Flight Mode")
	bool Hovering_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Flight Mode")
	bool Aerial_ = false;

public:
	FALSFlightMode()
	{

	}

	FALSFlightMode(const EALSFlightMode InitialFlightMode) { *this = InitialFlightMode; }

	const bool& None() const { return None_; }
	const bool& Hovering() const { return Hovering_; }
	const bool& Aerial() const { return Aerial_; }

	operator EALSFlightMode() const { return FlightMode; }

	void operator=(const EALSFlightMode InitialFlightMode)
	{
		FlightMode = InitialFlightMode;
		None_ = FlightMode == EALSFlightMode::None;
		Hovering_ = FlightMode == EALSFlightMode::Hovering;
		Aerial_ = FlightMode == EALSFlightMode::Aerial;
	}
};

USTRUCT(BlueprintType)
struct FALSStance
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	EALSStance Stance = EALSStance::Standing;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Standing_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Crouching_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Riding_ = false;

public:
	FALSStance()
	{
	}

	FALSStance(const EALSStance InitialStance) { *this = InitialStance; }

	const bool& Standing() const { return Standing_; }
	const bool& Crouching() const { return Crouching_; }
	const bool& Riding() const { return Riding_; }

	operator EALSStance() const { return Stance; }

	void operator=(const EALSStance NewStance)
	{
		Stance = NewStance;
		Standing_ = Stance == EALSStance::Standing;
		Crouching_ = Stance == EALSStance::Crouching;
		Riding_ = Stance == EALSStance::Riding;
	}
};

USTRUCT(BlueprintType)
struct FALSRotationMode
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Rotation System")
	EALSRotationMode RotationMode = EALSRotationMode::VelocityDirection;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Rotation System")
	bool VelocityDirection_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Rotation System")
	bool LookingDirection_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Rotation System")
	bool Aiming_ = false;

public:
	FALSRotationMode()
	{
	}

	FALSRotationMode(const EALSRotationMode InitialRotationMode) { *this = InitialRotationMode; }

	const bool& VelocityDirection() const { return VelocityDirection_; }
	const bool& LookingDirection() const { return LookingDirection_; }
	const bool& Aiming() const { return Aiming_; }

	operator EALSRotationMode() const { return RotationMode; }

	void operator=(const EALSRotationMode NewRotationMode)
	{
		RotationMode = NewRotationMode;
		VelocityDirection_ = RotationMode == EALSRotationMode::VelocityDirection;
		LookingDirection_ = RotationMode == EALSRotationMode::LookingDirection;
		Aiming_ = RotationMode == EALSRotationMode::Aiming;
	}
};

USTRUCT(BlueprintType)
struct FALSMovementDirection
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	EALSMovementDirection MovementDirection = EALSMovementDirection::Forward;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Forward_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Right_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Left_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Backward_ = false;

public:
	FALSMovementDirection()
	{
	}

	FALSMovementDirection(const EALSMovementDirection InitialMovementDirection)
	{
		*this = InitialMovementDirection;
	}

	const bool& Forward() const { return Forward_; }
	const bool& Right() const { return Right_; }
	const bool& Left() const { return Left_; }
	const bool& Backward() const { return Backward_; }

	operator EALSMovementDirection() const { return MovementDirection; }

	void operator=(const EALSMovementDirection NewMovementDirection)
	{
		MovementDirection = NewMovementDirection;
		Forward_ = MovementDirection == EALSMovementDirection::Forward;
		Right_ = MovementDirection == EALSMovementDirection::Right;
		Left_ = MovementDirection == EALSMovementDirection::Left;
		Backward_ = MovementDirection == EALSMovementDirection::Backward;
	}
};

USTRUCT(BlueprintType)
struct FALSMovementAction
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	EALSMovementAction Action = EALSMovementAction::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool None_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool LowMantle_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool HighMantle_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Rolling_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool GettingUp_ = false;

public:
	FALSMovementAction()
	{
	}

	FALSMovementAction(const EALSMovementAction InitialAction) { *this = InitialAction; }

	const bool& None() const { return None_; }
	const bool& LowMantle() const { return LowMantle_; }
	const bool& HighMantle() const { return HighMantle_; }
	const bool& Rolling() const { return Rolling_; }
	const bool& GettingUp() const { return GettingUp_; }

	operator EALSMovementAction() const { return Action; }

	void operator=(const EALSMovementAction NewAction)
	{
		Action = NewAction;
		None_ = Action == EALSMovementAction::None;
		LowMantle_ = Action == EALSMovementAction::LowMantle;
		HighMantle_ = Action == EALSMovementAction::HighMantle;
		Rolling_ = Action == EALSMovementAction::Rolling;
		GettingUp_ = Action == EALSMovementAction::GettingUp;
	}
};

USTRUCT(BlueprintType)
struct FALSGait
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	EALSGait Gait = EALSGait::Walking;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Walking_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Running_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Sprinting_ = false;

public:
	FALSGait()
	{
	}

	FALSGait(const EALSGait InitialGait) { *this = InitialGait; }

	const bool& Walking() const { return Walking_; }
	const bool& Running() const { return Running_; }
	const bool& Sprinting() const { return Sprinting_; }

	operator EALSGait() const { return Gait; }

	void operator=(const EALSGait NewGait)
	{
		Gait = NewGait;
		Walking_ = Gait == EALSGait::Walking;
		Running_ = Gait == EALSGait::Running;
		Sprinting_ = Gait == EALSGait::Sprinting;
	}
};

USTRUCT(BlueprintType)
struct FALSOverlayState
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	EALSOverlayState State = EALSOverlayState::Default;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Default_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Masculine_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Feminine_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Injured_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool HandsTied_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Rifle_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool PistolOneHanded_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool PistolTwoHanded_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Bow_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Torch_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Binoculars_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Box_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Barrel_ = false;

public:
	FALSOverlayState()
	{
	}

	FALSOverlayState(const EALSOverlayState InitialState) { *this = InitialState; }

	const bool& Default() const { return Default_; }
	const bool& Masculine() const { return Masculine_; }
	const bool& Feminine() const { return Feminine_; }
	const bool& Injured() const { return Injured_; }
	const bool& HandsTied() const { return HandsTied_; }
	const bool& Rifle() const { return Rifle_; }
	const bool& PistolOneHanded() const { return PistolOneHanded_; }
	const bool& PistolTwoHanded() const { return PistolTwoHanded_; }
	const bool& Bow() const { return Bow_; }
	const bool& Torch() const { return Torch_; }
	const bool& Binoculars() const { return Binoculars_; }
	const bool& Box() const { return Box_; }
	const bool& Barrel() const { return Barrel_; }

	operator EALSOverlayState() const { return State; }

	void operator=(const EALSOverlayState NewAction)
	{
		State = NewAction;
		Default_ = State == EALSOverlayState::Default;
		Masculine_ = State == EALSOverlayState::Masculine;
		Feminine_ = State == EALSOverlayState::Feminine;
		Injured_ = State == EALSOverlayState::Injured;
		HandsTied_ = State == EALSOverlayState::HandsTied;
		Rifle_ = State == EALSOverlayState::Rifle;
		PistolOneHanded_ = State == EALSOverlayState::PistolOneHanded;
		PistolTwoHanded_ = State == EALSOverlayState::PistolTwoHanded;
		Bow_ = State == EALSOverlayState::Bow;
		Torch_ = State == EALSOverlayState::Torch;
		Binoculars_ = State == EALSOverlayState::Binoculars;
		Box_ = State == EALSOverlayState::Box;
		Barrel_ = State == EALSOverlayState::Barrel;
	}
};

USTRUCT(BlueprintType)
struct FALSGroundedEntryState
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Breakfall System")
	EALSGroundedEntryState State = EALSGroundedEntryState::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Breakfall System")
	bool None_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Breakfall System")
	bool Roll_ = false;

public:
	FALSGroundedEntryState()
	{
	}

	FALSGroundedEntryState(const EALSGroundedEntryState InitialState) { *this = InitialState; }

	const bool& None() const { return None_; }
	const bool& Roll() const { return Roll_; }

	operator EALSGroundedEntryState() const { return State; }

	void operator=(const EALSGroundedEntryState NewState)
	{
		State = NewState;
		None_ = State == EALSGroundedEntryState::None;
		Roll_ = State == EALSGroundedEntryState::Roll;
	}
};
