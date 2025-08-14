#include "PlatformCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TuningData/CharacterTuningData.h"
#include "TuningData/CameraTuningData.h"

APlatformCharacter::APlatformCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create spring arm component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	// Create camera component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// Don't rotate when the controller rotates
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void APlatformCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Apply tuning data
	ApplyCharacterTuning();
	ApplyCameraTuning();
}

void APlatformCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast to Enhanced Input Component
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlatformCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlatformCharacter::Look);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlatformCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlatformCharacter::StopSprint);

		// Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlatformCharacter::Interact);
	}
}

void APlatformCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward direction
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// Get right direction
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlatformCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlatformCharacter::StartSprint()
{
	if (!bIsSprinting)
	{
		bIsSprinting = true;
		
		if (CharacterTuning)
		{
			GetCharacterMovement()->MaxWalkSpeed = CharacterTuning->SprintSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = 750.0f; // Default sprint speed
		}
	}
}

void APlatformCharacter::StopSprint()
{
	if (bIsSprinting)
	{
		bIsSprinting = false;
		
		if (CharacterTuning)
		{
			GetCharacterMovement()->MaxWalkSpeed = CharacterTuning->WalkSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = 500.0f; // Default walk speed
		}
	}
}

void APlatformCharacter::Interact()
{
	// TODO: Implement interaction logic in Milestone 2
	UE_LOG(LogTemp, Log, TEXT("Interact button pressed - implement in Milestone 2"));
}

void APlatformCharacter::ApplyCharacterTuning()
{
	if (CharacterTuning && GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = CharacterTuning->WalkSpeed;
		GetCharacterMovement()->JumpZVelocity = CharacterTuning->JumpHeight;
		GetCharacterMovement()->AirControl = CharacterTuning->AirControl;
		GetCharacterMovement()->BrakingDecelerationWalking = CharacterTuning->BrakingDeceleration;
		MaxJumpCount = CharacterTuning->MaxJumpCount;
	}
}

void APlatformCharacter::ApplyCameraTuning()
{
	if (CameraTuning && SpringArmComponent)
	{
		SpringArmComponent->TargetArmLength = CameraTuning->ArmLength;
		SpringArmComponent->CameraLagSpeed = CameraTuning->LagSpeed;
		SpringArmComponent->bEnableCameraLag = CameraTuning->bEnableLag;
		SpringArmComponent->CameraLagMaxDistance = CameraTuning->LagMaxDistance;
		SpringArmComponent->SocketOffset = CameraTuning->SocketOffset;
	}
}
