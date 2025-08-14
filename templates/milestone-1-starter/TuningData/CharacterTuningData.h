#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterTuningData.generated.h"

UCLASS(BlueprintType)
class PLATFORMER_API UCharacterTuningData : public UDataAsset
{
	GENERATED_BODY()

public:
	UCharacterTuningData();

	// Movement speeds
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", ClampMax = "2000.0"))
	float WalkSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", ClampMax = "3000.0"))
	float SprintSpeed = 750.0f;

	// Jump settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jumping", meta = (ClampMin = "0.0", ClampMax = "2000.0"))
	float JumpHeight = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jumping", meta = (ClampMin = "1", ClampMax = "5"))
	int32 MaxJumpCount = 2;

	// Air control
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AirControl = 0.35f;

	// Braking
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", ClampMax = "5000.0"))
	float BrakingDeceleration = 2000.0f;

	// Rotation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
	float RotationRate = 500.0f;
};
