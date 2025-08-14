#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CameraTuningData.generated.h"

UCLASS(BlueprintType)
class PLATFORMER_API UCameraTuningData : public UDataAsset
{
	GENERATED_BODY()

public:
	UCameraTuningData();

	// Spring arm settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Distance", meta = (ClampMin = "50.0", ClampMax = "1000.0"))
	float ArmLength = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Position")
	FVector SocketOffset = FVector(0.0f, 0.0f, 50.0f);

	// Camera lag (smoothing)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Smoothing")
	bool bEnableLag = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Smoothing", meta = (ClampMin = "0.1", ClampMax = "25.0"))
	float LagSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Smoothing", meta = (ClampMin = "0.0", ClampMax = "500.0"))
	float LagMaxDistance = 200.0f;

	// Field of view
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera View", meta = (ClampMin = "30.0", ClampMax = "120.0"))
	float FieldOfView = 90.0f;

	// Camera collision
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Collision")
	bool bDoCollisionTest = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Collision", meta = (ClampMin = "1.0", ClampMax = "50.0"))
	float ProbeSize = 12.0f;
};
