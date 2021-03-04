// Project:         Advanced Locomotion System V4 on C++
// Copyright:       Copyright (C) 2021 Doğa Can Yanıkoğlu
// License:         MIT License (http://www.opensource.org/licenses/mit-license.php)
// Source Code:     https://github.com/dyanikoglu/ALSV4_CPP
// Original Author: Haziq Fadhil
// Contributors:    Doğa Can Yanıkoğlu


#include "Character/ALSCharacterMovementComponent.h"
#include "Character/ALSBaseCharacter.h"

#include "Curves/CurveVector.h"

UALSCharacterMovementComponent::UALSCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UALSCharacterMovementComponent::OnMovementUpdated(const float DeltaTime, const FVector& OldLocation,
                                                       const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaTime, OldLocation, OldVelocity);

	if (!CharacterOwner)
	{
		return;
	}

	// Set Movement Settings
	if (bRequestMovementSettingsChange)
	{
		MaxWalkSpeed = NewMaxWalkSpeed;
		MaxWalkSpeedCrouched = NewMaxWalkSpeed;
		MaxFlySpeed = NewMaxFlySpeed;
		MaxSwimSpeed = NewMaxSwimSpeed;
	}
}

void UALSCharacterMovementComponent::PhysWalking(const float deltaTime, const int32 Iterations)
{
	if (CurrentMovementSettings.MovementCurve)
	{
		// Update the Ground Friction using the Movement Curve.
		// This allows for fine control over movement behavior at each speed.
		GroundFriction = CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed()).Z;
	}
	Super::PhysWalking(deltaTime, Iterations);
}

float UALSCharacterMovementComponent::GetMaxAcceleration() const
{
	// Update the Acceleration using the Movement Curve.
	// This allows for fine control over movement behavior at each speed.
	if (!IsMovingOnGround() || !CurrentMovementSettings.MovementCurve)
	{
		return Super::GetMaxAcceleration();
	}
	return CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed()).X;
}

float UALSCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	// Update the Deceleration using the Movement Curve.
	// This allows for fine control over movement behavior at each speed.
	if (!IsMovingOnGround() || !CurrentMovementSettings.MovementCurve)
	{
		return Super::GetMaxBrakingDeceleration();
	}
	return CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed()).Y;
}

void UALSCharacterMovementComponent::UpdateFromCompressedFlags(const uint8 Flags) // Client only
{
	Super::UpdateFromCompressedFlags(Flags);

	bRequestMovementSettingsChange = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

class FNetworkPredictionData_Client* UALSCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);

	if (!ClientPredictionData)
	{
		UALSCharacterMovementComponent* MutableThis = const_cast<UALSCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_My(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UALSCharacterMovementComponent::FSavedMove_My::Clear()
{
	Super::Clear();

	bSavedRequestMovementSettingsChange = false;
}

uint8 UALSCharacterMovementComponent::FSavedMove_My::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSavedRequestMovementSettingsChange)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

void UALSCharacterMovementComponent::FSavedMove_My::SetMoveFor(ACharacter* Character, const float InDeltaTime,
                                                               FVector const& NewAccel,
                                                               class FNetworkPredictionData_Client_Character&
                                                               ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UALSCharacterMovementComponent* CharacterMovement = Cast<UALSCharacterMovementComponent>(
		Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		bSavedRequestMovementSettingsChange = CharacterMovement->bRequestMovementSettingsChange;
		switch (CharacterMovement->MovementMode)
		{
			case MOVE_Walking: MaxSpeed = CharacterMovement->NewMaxWalkSpeed;
			case MOVE_Flying: MaxSpeed = CharacterMovement->NewMaxFlySpeed;
			case MOVE_Swimming: MaxSpeed = CharacterMovement->NewMaxSwimSpeed;
			default: MaxSpeed = CharacterMovement->NewMaxWalkSpeed;
		}

	}
}

UALSCharacterMovementComponent::FNetworkPredictionData_Client_My::FNetworkPredictionData_Client_My(
	const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement) {}

FSavedMovePtr UALSCharacterMovementComponent::FNetworkPredictionData_Client_My::AllocateNewMove()
{
	return MakeShared<FSavedMove_My>();
}

float UALSCharacterMovementComponent::GetMappedSpeed() const
{
	// Map the character's current speed to the configured movement speeds with a range of 0-3,
	// with 0 = stopped, 1 = the Slow Speed, 2 = the Normal Speed, and 3 = the Fast Speed.
	// This allows us to vary the movement speeds but still use the mapped range in calculations for consistent results

	const float Speed = Velocity.Size2D();
	const float LocSlowSpeed = CurrentMovementSettings.SlowSpeed;
	const float LocNormalSpeed = CurrentMovementSettings.NormalSpeed;
	const float LocFastSpeed = CurrentMovementSettings.FastSpeed;

	if (Speed > LocNormalSpeed)
	{
		return FMath::GetMappedRangeValueClamped({LocNormalSpeed, LocFastSpeed}, {2.0f, 3.0f}, Speed);
	}

	if (Speed > LocSlowSpeed)
	{
		return FMath::GetMappedRangeValueClamped({LocSlowSpeed, LocNormalSpeed}, {1.0f, 2.0f}, Speed);
	}

	return FMath::GetMappedRangeValueClamped({0.0f, LocSlowSpeed}, {0.0f, 1.0f}, Speed);
}

void UALSCharacterMovementComponent::SetMovementSettings(const FALSMovementSettings NewMovementSettings)
{
	// Set the current movement settings from the owner
	CurrentMovementSettings = NewMovementSettings;
}

void UALSCharacterMovementComponent::Server_SetMaxWalkingSpeed_Implementation(const float UpdateMaxWalkSpeed)
{
	NewMaxWalkSpeed = UpdateMaxWalkSpeed;
}

void UALSCharacterMovementComponent::SetMaxWalkingSpeed(const float UpdateMaxWalkSpeed)
{
	if (UpdateMaxWalkSpeed != NewMaxWalkSpeed)
	{
		if (PawnOwner->IsLocallyControlled())
		{
			NewMaxWalkSpeed = UpdateMaxWalkSpeed;
			Server_SetMaxWalkingSpeed(UpdateMaxWalkSpeed);
			bRequestMovementSettingsChange = true;
			return;
		}
		if (!PawnOwner->HasAuthority())
		{
			MaxWalkSpeed = UpdateMaxWalkSpeed;
			MaxWalkSpeedCrouched = UpdateMaxWalkSpeed;
		}
	}
}

void UALSCharacterMovementComponent::Server_SetMaxFlyingSpeed_Implementation(const float UpdateMaxFlySpeed)
{
	NewMaxFlySpeed = UpdateMaxFlySpeed;
}

void UALSCharacterMovementComponent::SetMaxFlyingSpeed(const float UpdateMaxFlySpeed)
{
	if (UpdateMaxFlySpeed != NewMaxFlySpeed)
	{
		if (PawnOwner->IsLocallyControlled())
		{
			NewMaxFlySpeed = UpdateMaxFlySpeed;
			Server_SetMaxFlyingSpeed(UpdateMaxFlySpeed);
			bRequestMovementSettingsChange = true;
			return;
		}
		if (!PawnOwner->HasAuthority())
		{
			MaxFlySpeed = UpdateMaxFlySpeed;
		}
	}
}

void UALSCharacterMovementComponent::Server_SetMaxSwimmingSpeed_Implementation(const float UpdateMaxSwimSpeed)
{
	NewMaxSwimSpeed = UpdateMaxSwimSpeed;
}

void UALSCharacterMovementComponent::SetMaxSwimmingSpeed(const float UpdateMaxSwimSpeed)
{
	if (UpdateMaxSwimSpeed != NewMaxSwimSpeed)
	{
		if (PawnOwner->IsLocallyControlled())
		{
			NewMaxSwimSpeed = UpdateMaxSwimSpeed;
			Server_SetMaxSwimmingSpeed(UpdateMaxSwimSpeed);
			bRequestMovementSettingsChange = true;
			return;
		}
		if (!PawnOwner->HasAuthority())
		{
			MaxSwimSpeed = UpdateMaxSwimSpeed;
		}
	}
}