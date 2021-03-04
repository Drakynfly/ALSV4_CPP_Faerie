#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/NoExportTypes.h"
#include "ALS_Settings.generated.h"

/**
* Configurable settings for the ALSV4_Faerie plugin.
*/
UCLASS(Config = "Project", defaultconfig, meta = (DisplayName = "Faerie ALS"))
class ALSV4_CPP_API UALS_Settings final : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// Axis input for forward and backward movement. You should probably bind this one.
	UPROPERTY(EditAnywhere, Config, Category = "Input")
	FName X_Axis_Input;

	// Axis input for left and right movement. Good idea to bind this one too.
	UPROPERTY(EditAnywhere, Config, Category = "Input")
	FName Y_Axis_Input;

	// Axis input for vertical movement. Only bind this one if you want to utilize the flight or swimming controls.
	UPROPERTY(EditAnywhere, Config, Category = "Input")
	FName Z_Axis_Input;

	// Vertical camera input axis.
	UPROPERTY(EditAnywhere, Config, Category = "Input")
	FName Pitch_Input;

	// Horizontal camera input axis.
	UPROPERTY(EditAnywhere, Config, Category = "Input")
	FName Yaw_Input;

	// Camera spin controls. Lmao, this is funny to mess with but not very useful, unless you are trying something really crazy.
	UPROPERTY(EditAnywhere, Config, Category = "Input")
	FName Roll_Input;

	/**
	* This is the channel that the mantle check function will use to determine if something can be mantled.
	* WorldStatic will work fine, or you can create a custom channel for more precise control.
	*/
	UPROPERTY(EditAnywhere, Config, Category = "Mantling")
	TEnumAsByte<ECollisionChannel> MantleCheckChannel;

	/**
	* This channel is used by the flight system to determine the altitude from the ground. WorldStatic works, but if
	* you have a landscape or other large mass that you want to count as ground, then you can change this.
	*/
	UPROPERTY(EditAnywhere, Config, Category = "Flight")
	TEnumAsByte<ECollisionChannel> FlightCheckChannel;

	// Height of sea level, in world space.
	UPROPERTY(EditDefaultsOnly, Category = "Flight")
	float SeaAltitude = 0.f;

	/**
	 * Height of the troposphere. Represents the max altitude that flight can be maintained at. This is in world space,
	 * not relative to sea level. E.g, if you want a 100,000 unit tall atmosphere, but the sea altitude is set to -1,000,
	 * then you need to set this to 99,000.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Flight")
	float TroposphereHeight = 1000000.f;

	static FORCEINLINE UALS_Settings* Get()
	{
		UALS_Settings* Settings = GetMutableDefault<UALS_Settings>();
		check(Settings);

		return Settings;
	}
};