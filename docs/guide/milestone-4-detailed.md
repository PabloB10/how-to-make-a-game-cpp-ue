# Milestone 4 - Detailed Implementation

**Goal:** Create friendly NPCs with dialog system and interactive events for world storytelling

**Estimated Time:** 4-6 hours over 2 sessions

## Overview Checklist

- [ ] Create AFriendlyNPC with dialog capabilities
- [ ] Implement ITalkable interface for conversations
- [ ] Build simple dialog system with UI
- [ ] Create ATriggerEventActor for world events
- [ ] Set up event routing via subsystem
- [ ] Test NPC interactions and triggered events

---

## Session 1: Friendly NPCs & Dialog System (2-3 hours)

### Step 4.1: Create ITalkable Interface (30 minutes)

**Create Interface:**

1. Tools → New C++ Class → Interface → Next
2. Name: `Talkable`

**Header `Source/Platformer/Talkable.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Talkable.generated.h"

USTRUCT(BlueprintType)
struct FDialogLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	FText SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog", meta = (MultiLine = true))
	FText DialogText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	float DisplayDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	bool bWaitForInput = true;

	FDialogLine()
	{
		SpeakerName = FText::FromString(TEXT("Unknown"));
		DialogText = FText::FromString(TEXT("..."));
		DisplayDuration = 3.0f;
		bWaitForInput = true;
	}

	FDialogLine(const FText& InSpeakerName, const FText& InDialogText, bool bInWaitForInput = true)
		: SpeakerName(InSpeakerName), DialogText(InDialogText), bWaitForInput(bInWaitForInput)
	{
		DisplayDuration = 3.0f;
	}
};

USTRUCT(BlueprintType)
struct FDialogSession
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	TArray<FDialogLine> DialogLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	bool bCanRepeat = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	bool bHasBeenSeen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	FString DialogID;

	FDialogSession()
	{
		bCanRepeat = true;
		bHasBeenSeen = false;
		DialogID = TEXT("DefaultDialog");
	}
};

UINTERFACE(MinimalAPI, Blueprintable)
class UTalkable : public UInterface
{
	GENERATED_BODY()
};

class PLATFORMER_API ITalkable
{
	GENERATED_BODY()

public:
	// Check if this NPC can currently talk
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialog")
	bool CanTalk(AActor* Instigator) const;
	virtual bool CanTalk_Implementation(AActor* Instigator) const { return true; }

	// Start dialog with this NPC
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialog")
	void StartDialog(AActor* Instigator);
	virtual void StartDialog_Implementation(AActor* Instigator) {}

	// Get the current dialog session
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialog")
	FDialogSession GetCurrentDialog() const;
	virtual FDialogSession GetCurrentDialog_Implementation() const { return FDialogSession(); }

	// Check if dialog is currently active
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialog")
	bool IsDialogActive() const;
	virtual bool IsDialogActive_Implementation() const { return false; }

	// End current dialog
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialog")
	void EndDialog();
	virtual void EndDialog_Implementation() {}

	// Get interaction priority for talkable objects
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialog")
	int32 GetTalkPriority() const;
	virtual int32 GetTalkPriority_Implementation() const { return 10; } // Higher than regular interactables
};
```

**Implementation `Source/Platformer/Talkable.cpp`:**

```cpp
#include "Talkable.h"

// Interface implementation is in header
```

### Step 4.2: Create AFriendlyNPC Class (60 minutes)

**Create NPC Class:**

1. Tools → New C++ Class → Character → Next
2. Name: `FriendlyNPC`

**Header `Source/Platformer/FriendlyNPC.h`:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interactable.h"
#include "Talkable.h"
#include "FriendlyNPC.generated.h"

class UStatsComponent;
class UWidgetComponent;

UCLASS()
class PLATFORMER_API AFriendlyNPC : public ACharacter, public IInteractable, public ITalkable
{
	GENERATED_BODY()

public:
	AFriendlyNPC();

protected:
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStatsComponent* StatsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* NameplateWidget;

	// NPC Identity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FText NPCName = FText::FromString(TEXT("Friendly NPC"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FText NPCTitle = FText::FromString(TEXT(""));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FText ShortDescription = FText::FromString(TEXT("A helpful character"));

	// Dialog Configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	TArray<FDialogSession> DialogSessions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	int32 CurrentDialogIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	bool bCanTalkMultipleTimes = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
	float DialogCooldown = 1.0f;

	// Interaction Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bCanInteract = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	int32 InteractionPriority = 10;

	// State
	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsInDialog = false;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	AActor* CurrentDialogPartner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	float LastInteractionTime = 0.0f;

public:
	// IInteractable interface
	virtual FText GetInteractionVerb_Implementation() const override;
	virtual FText GetInteractionName_Implementation() const override;
	virtual bool CanInteract_Implementation(AActor* Instigator) const override;
	virtual void Interact_Implementation(AActor* Instigator) override;
	virtual int32 GetInteractionPriority_Implementation() const override;

	// ITalkable interface
	virtual bool CanTalk_Implementation(AActor* Instigator) const override;
	virtual void StartDialog_Implementation(AActor* Instigator) override;
	virtual FDialogSession GetCurrentDialog_Implementation() const override;
	virtual bool IsDialogActive_Implementation() const override;
	virtual void EndDialog_Implementation() override;
	virtual int32 GetTalkPriority_Implementation() const override;

	// Dialog management
	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void AddDialogSession(const FDialogSession& NewSession);

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void SetCurrentDialogIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	bool HasMoreDialogs() const;

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void AdvanceToNextDialog();

protected:
	// Blueprint events
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialog")
	void OnDialogStarted(AActor* Partner);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialog")
	void OnDialogEnded(AActor* Partner);

	UFUNCTION(BlueprintNativeEvent, Category = "Dialog")
	void HandleDialogStart(AActor* Partner);
	virtual void HandleDialogStart_Implementation(AActor* Partner);

	UFUNCTION(BlueprintNativeEvent, Category = "Dialog")
	void HandleDialogEnd(AActor* Partner);
	virtual void HandleDialogEnd_Implementation(AActor* Partner);
};
```

**Implementation `Source/Platformer/FriendlyNPC.cpp`:**

```cpp
#include "FriendlyNPC.h"
#include "StatsComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

AFriendlyNPC::AFriendlyNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create stats component (for consistency, even if not used for combat)
	StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));

	// Create nameplate widget
	NameplateWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameplateWidget"));
	NameplateWidget->SetupAttachment(RootComponent);
	NameplateWidget->SetRelativeLocation(FVector(0, 0, 90));
	NameplateWidget->SetWidgetSpace(EWidgetSpace::Screen);

	// Configure character movement (slower than player)
	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure collision
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Setup default dialog
	FDialogSession DefaultDialog;
	DefaultDialog.DialogID = TEXT("Greeting");
	DefaultDialog.DialogLines.Add(FDialogLine(
		NPCName,
		FText::FromString(TEXT("Hello there! How can I help you?")),
		true
	));
	DialogSessions.Add(DefaultDialog);
}

void AFriendlyNPC::BeginPlay()
{
	Super::BeginPlay();

	// Validate setup
	if (DialogSessions.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("FriendlyNPC %s has no dialog sessions!"), *GetName());
	}

	UE_LOG(LogTemp, Log, TEXT("FriendlyNPC %s spawned with %d dialog sessions"),
		*GetName(), DialogSessions.Num());
}

// IInteractable Implementation
FText AFriendlyNPC::GetInteractionVerb_Implementation() const
{
	return FText::FromString(TEXT("Talk"));
}

FText AFriendlyNPC::GetInteractionName_Implementation() const
{
	return NPCName;
}

bool AFriendlyNPC::CanInteract_Implementation(AActor* Instigator) const
{
	if (!bCanInteract || bIsInDialog)
		return false;

	// Check cooldown
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastInteractionTime < DialogCooldown)
		return false;

	return CanTalk_Implementation(Instigator);
}

void AFriendlyNPC::Interact_Implementation(AActor* Instigator)
{
	if (!CanInteract_Implementation(Instigator))
		return;

	StartDialog_Implementation(Instigator);
}

int32 AFriendlyNPC::GetInteractionPriority_Implementation() const
{
	return InteractionPriority;
}

// ITalkable Implementation
bool AFriendlyNPC::CanTalk_Implementation(AActor* Instigator) const
{
	if (bIsInDialog && CurrentDialogPartner != Instigator)
		return false; // Already talking to someone else

	if (DialogSessions.Num() == 0)
		return false; // No dialogs available

	// Check if current dialog can be repeated
	if (CurrentDialogIndex < DialogSessions.Num())
	{
		const FDialogSession& CurrentSession = DialogSessions[CurrentDialogIndex];
		if (CurrentSession.bHasBeenSeen && !CurrentSession.bCanRepeat)
		{
			// This dialog has been seen and can't repeat, check if we have more
			return HasMoreDialogs();
		}
	}

	return true;
}

void AFriendlyNPC::StartDialog_Implementation(AActor* Instigator)
{
	if (!CanTalk_Implementation(Instigator))
		return;

	bIsInDialog = true;
	CurrentDialogPartner = Instigator;
	LastInteractionTime = GetWorld()->GetTimeSeconds();

	// Mark current dialog as seen
	if (CurrentDialogIndex < DialogSessions.Num())
	{
		DialogSessions[CurrentDialogIndex].bHasBeenSeen = true;
	}

	HandleDialogStart(Instigator);
	OnDialogStarted(Instigator);

	UE_LOG(LogTemp, Log, TEXT("FriendlyNPC %s started dialog with %s"),
		*GetName(), Instigator ? *Instigator->GetName() : TEXT("Unknown"));
}

FDialogSession AFriendlyNPC::GetCurrentDialog_Implementation() const
{
	if (CurrentDialogIndex < DialogSessions.Num())
	{
		return DialogSessions[CurrentDialogIndex];
	}

	return FDialogSession(); // Return empty dialog if none available
}

bool AFriendlyNPC::IsDialogActive_Implementation() const
{
	return bIsInDialog;
}

void AFriendlyNPC::EndDialog_Implementation()
{
	if (!bIsInDialog)
		return;

	AActor* Partner = CurrentDialogPartner;
	bIsInDialog = false;
	CurrentDialogPartner = nullptr;

	HandleDialogEnd(Partner);
	OnDialogEnded(Partner);

	UE_LOG(LogTemp, Log, TEXT("FriendlyNPC %s ended dialog"), *GetName());
}

int32 AFriendlyNPC::GetTalkPriority_Implementation() const
{
	return InteractionPriority;
}

// Dialog Management
void AFriendlyNPC::AddDialogSession(const FDialogSession& NewSession)
{
	DialogSessions.Add(NewSession);
}

void AFriendlyNPC::SetCurrentDialogIndex(int32 NewIndex)
{
	CurrentDialogIndex = FMath::Clamp(NewIndex, 0, DialogSessions.Num() - 1);
}

bool AFriendlyNPC::HasMoreDialogs() const
{
	// Check if there are any unseen dialogs or repeatable dialogs ahead
	for (int32 i = CurrentDialogIndex + 1; i < DialogSessions.Num(); i++)
	{
		const FDialogSession& Session = DialogSessions[i];
		if (!Session.bHasBeenSeen || Session.bCanRepeat)
		{
			return true;
		}
	}
	return false;
}

void AFriendlyNPC::AdvanceToNextDialog()
{
	if (HasMoreDialogs())
	{
		CurrentDialogIndex++;

		// Skip to next available dialog
		while (CurrentDialogIndex < DialogSessions.Num())
		{
			const FDialogSession& Session = DialogSessions[CurrentDialogIndex];
			if (!Session.bHasBeenSeen || Session.bCanRepeat)
			{
				break;
			}
			CurrentDialogIndex++;
		}
	}
}

void AFriendlyNPC::HandleDialogStart_Implementation(AActor* Partner)
{
	// Face the dialog partner
	if (Partner)
	{
		FVector DirectionToPartner = (Partner->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FRotator LookAtRotation = DirectionToPartner.Rotation();
		LookAtRotation.Pitch = 0; // Keep upright
		SetActorRotation(LookAtRotation);
	}
}

void AFriendlyNPC::HandleDialogEnd_Implementation(AActor* Partner)
{
	// Advance to next dialog if available and configured to do so
	if (bCanTalkMultipleTimes && HasMoreDialogs())
	{
		AdvanceToNextDialog();
	}
}
```

### Step 4.3: Create Dialog UI System (60 minutes)

**Create Dialog Widget:**

Create `Source/Platformer/DialogWidget.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Talkable.h"
#include "DialogWidget.generated.h"

class UTextBlock;
class UButton;
class URichTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogLineChanged, int32, LineIndex);

UCLASS()
class PLATFORMER_API UDialogWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	// Widget bindings
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeakerNameText;

	UPROPERTY(meta = (BindWidget))
	URichTextBlock* DialogText;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ContinuePrompt;

	// Current dialog state
	UPROPERTY(BlueprintReadOnly, Category = "Dialog")
	FDialogSession CurrentSession;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog")
	int32 CurrentLineIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog")
	bool bIsDialogActive = false;

	UPROPERTY(BlueprintReadOnly, Category = "Dialog")
	AActor* CurrentNPC = nullptr;

public:
	// Events
	UPROPERTY(BlueprintAssignable, Category = "Dialog")
	FOnDialogComplete OnDialogComplete;

	UPROPERTY(BlueprintAssignable, Category = "Dialog")
	FOnDialogLineChanged OnDialogLineChanged;

	// Public functions
	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void StartDialog(AActor* NPC, const FDialogSession& DialogSession);

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void EndDialog();

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	void AdvanceDialog();

	UFUNCTION(BlueprintCallable, Category = "Dialog")
	bool IsDialogActive() const { return bIsDialogActive; }

protected:
	UFUNCTION()
	void OnContinueButtonClicked();

private:
	void DisplayCurrentLine();
	void SetupNextLine();
	FTimerHandle AutoAdvanceTimerHandle;
};
```

**Implementation `Source/Platformer/DialogWidget.cpp`:**

```cpp
#include "DialogWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/RichTextBlock.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Talkable.h"

void UDialogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button event
	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UDialogWidget::OnContinueButtonClicked);
	}

	// Initially hidden
	SetVisibility(ESlateVisibility::Hidden);
}

FReply UDialogWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bIsDialogActive)
	{
		// Space, Enter, or E advances dialog
		if (InKeyEvent.GetKey() == EKeys::SpaceBar ||
			InKeyEvent.GetKey() == EKeys::Enter ||
			InKeyEvent.GetKey() == EKeys::E)
		{
			AdvanceDialog();
			return FReply::Handled();
		}

		// Escape ends dialog
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			EndDialog();
			return FReply::Handled();
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UDialogWidget::StartDialog(AActor* NPC, const FDialogSession& DialogSession)
{
	if (!NPC || DialogSession.DialogLines.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("DialogWidget: Invalid dialog session"));
		return;
	}

	CurrentNPC = NPC;
	CurrentSession = DialogSession;
	CurrentLineIndex = 0;
	bIsDialogActive = true;

	SetVisibility(ESlateVisibility::Visible);
	DisplayCurrentLine();

	UE_LOG(LogTemp, Log, TEXT("DialogWidget: Started dialog with %d lines"), CurrentSession.DialogLines.Num());
}

void UDialogWidget::EndDialog()
{
	if (!bIsDialogActive)
		return;

	// Clear timer
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
	}

	// End dialog on NPC
	if (CurrentNPC && CurrentNPC->Implements<UTalkable>())
	{
		ITalkable::Execute_EndDialog(CurrentNPC);
	}

	bIsDialogActive = false;
	CurrentNPC = nullptr;
	CurrentLineIndex = 0;

	SetVisibility(ESlateVisibility::Hidden);
	OnDialogComplete.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("DialogWidget: Dialog ended"));
}

void UDialogWidget::AdvanceDialog()
{
	if (!bIsDialogActive)
		return;

	CurrentLineIndex++;

	if (CurrentLineIndex >= CurrentSession.DialogLines.Num())
	{
		// Dialog finished
		EndDialog();
	}
	else
	{
		// Display next line
		DisplayCurrentLine();
		OnDialogLineChanged.Broadcast(CurrentLineIndex);
	}
}

void UDialogWidget::DisplayCurrentLine()
{
	if (CurrentLineIndex >= CurrentSession.DialogLines.Num())
		return;

	const FDialogLine& CurrentLine = CurrentSession.DialogLines[CurrentLineIndex];

	// Update UI
	if (SpeakerNameText)
	{
		SpeakerNameText->SetText(CurrentLine.SpeakerName);
	}

	if (DialogText)
	{
		DialogText->SetText(CurrentLine.DialogText);
	}

	// Setup continue prompt/button
	if (CurrentLine.bWaitForInput)
	{
		if (ContinueButton)
		{
			ContinueButton->SetVisibility(ESlateVisibility::Visible);
		}
		if (ContinuePrompt)
		{
			ContinuePrompt->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		// Auto-advance after duration
		if (ContinueButton)
		{
			ContinueButton->SetVisibility(ESlateVisibility::Hidden);
		}
		if (ContinuePrompt)
		{
			ContinuePrompt->SetVisibility(ESlateVisibility::Hidden);
		}

		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(
				AutoAdvanceTimerHandle,
				this,
				&UDialogWidget::AdvanceDialog,
				CurrentLine.DisplayDuration,
				false
			);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("DialogWidget: Displaying line %d: %s"),
		CurrentLineIndex, *CurrentLine.DialogText.ToString());
}

void UDialogWidget::OnContinueButtonClicked()
{
	AdvanceDialog();
}
```

### Step 4.4: Create Dialog UI Blueprint (30 minutes)

**Create Widget Blueprint:**

1. Content Browser → Add → User Interface → Widget Blueprint
2. Name: `WBP_Dialog`
3. Parent Class: `DialogWidget`

**Widget Layout:**

1. Canvas Panel (Root)
2. Border (Background - semi-transparent black)
   - Anchors: Bottom center
   - Size: 800x200
   - Position: Center bottom with margin
3. Vertical Box inside Border:
   - Text Block: `SpeakerNameText` (Bold, larger font)
   - Rich Text Block: `DialogText` (Main dialog text)
   - Horizontal Box:
     - Button: `ContinueButton` (Text: "Continue")
     - Text Block: `ContinuePrompt` (Text: "Press Space to continue...")

**Style the widget:**

- Background: Dark semi-transparent
- Text: White with good contrast
- Button: Styled consistently with game theme

### Step 4.5: Integrate Dialog System with Character (30 minutes)

**Update `PlatformCharacter.h`:**

```cpp
// Add include
#include "DialogWidget.h"

// Add to protected members
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
TSubclassOf<UDialogWidget> DialogWidgetClass;

UPROPERTY()
UDialogWidget* DialogWidget;

// Add functions
UFUNCTION()
void OnDialogComplete();

bool IsInDialog() const;
```

**Update `PlatformCharacter.cpp`:**

```cpp
// In BeginPlay:
if (DialogWidgetClass)
{
	DialogWidget = CreateWidget<UDialogWidget>(GetWorld(), DialogWidgetClass);
	if (DialogWidget)
	{
		DialogWidget->AddToViewport(10); // Higher Z-order than health UI
		DialogWidget->OnDialogComplete.AddDynamic(this, &APlatformCharacter::OnDialogComplete);
	}
}

// Update interaction component binding to handle dialog
if (InteractionComponent)
{
	InteractionComponent->OnFocusChanged.AddDynamic(this, &APlatformCharacter::OnInteractionFocusChanged);
}

// Update OnInteractionFocusChanged to handle talkable NPCs:
void APlatformCharacter::OnInteractionFocusChanged(AActor* NewFocus, AActor* OldFocus)
{
	if (!InteractionPromptWidget)
		return;

	if (NewFocus)
	{
		FText InteractionText;

		// Check if it's a talkable NPC first (higher priority)
		if (NewFocus->Implements<UTalkable>() && ITalkable::Execute_CanTalk(NewFocus, this))
		{
			FText NPCName = IInteractable::Execute_GetInteractionName(NewFocus);
			InteractionText = FText::Format(FText::FromString(TEXT("Talk {0}")), NPCName);
		}
		else if (NewFocus->Implements<UInteractable>())
		{
			InteractionText = InteractionComponent->GetCurrentInteractionText();
		}

		if (!InteractionText.IsEmpty())
		{
			InteractionPromptWidget->ShowPrompt(InteractionText);
		}
		else
		{
			InteractionPromptWidget->HidePrompt();
		}
	}
	else
	{
		InteractionPromptWidget->HidePrompt();
	}
}

// Update Interact function to handle dialogs:
void APlatformCharacter::Interact()
{
	if (!InteractionComponent)
		return;

	AActor* FocusActor = InteractionComponent->GetCurrentFocusActor();
	if (!FocusActor)
		return;

	// Handle talkable NPCs
	if (FocusActor->Implements<UTalkable>() && ITalkable::Execute_CanTalk(FocusActor, this))
	{
		if (DialogWidget)
		{
			FDialogSession DialogSession = ITalkable::Execute_GetCurrentDialog(FocusActor);
			DialogWidget->StartDialog(FocusActor, DialogSession);
			ITalkable::Execute_StartDialog(FocusActor, this);
		}
		return;
	}

	// Handle regular interactables
	InteractionComponent->TryInteract();
}

// Add implementations:
void APlatformCharacter::OnDialogComplete()
{
	UE_LOG(LogTemp, Log, TEXT("Dialog completed"));
	// Resume normal gameplay
}

bool APlatformCharacter::IsInDialog() const
{
	return DialogWidget && DialogWidget->IsDialogActive();
}
```

---

## Session 2: Trigger Events & World Interaction (2-3 hours)

### Step 4.6: Create Event Subsystem (45 minutes)

**Create Game Event Types:**

Create `Source/Platformer/GameplayEvents.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEvents.generated.h"

USTRUCT(BlueprintType)
struct FGameplayEventData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString EventID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FText EventName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FText EventDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	TMap<FString, FString> Parameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	AActor* Instigator = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FVector EventLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	float EventTime = 0.0f;

	FGameplayEventData()
	{
		EventID = TEXT("DefaultEvent");
		EventName = FText::FromString(TEXT("Unknown Event"));
		EventDescription = FText::FromString(TEXT(""));
		EventLocation = FVector::ZeroVector;
		EventTime = 0.0f;
		Instigator = nullptr;
	}

	FGameplayEventData(const FString& InEventID, AActor* InInstigator = nullptr)
		: EventID(InEventID), Instigator(InInstigator)
	{
		EventName = FText::FromString(InEventID);
		EventDescription = FText::FromString(TEXT(""));
		EventLocation = InInstigator ? InInstigator->GetActorLocation() : FVector::ZeroVector;
		EventTime = 0.0f;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayEvent, const FGameplayEventData&, EventData);

UCLASS(BlueprintType)
class PLATFORMER_API UGameplayEventAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
	FString EventID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
	FText EventName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event", meta = (MultiLine = true))
	FText EventDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
	bool bLogToConsole = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
	bool bShowOnScreen = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Event")
	float OnScreenDuration = 3.0f;
};
```

**Create Event Subsystem:**

Create `Source/Platformer/GameplayEventSubsystem.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayEvents.h"
#include "GameplayEventSubsystem.generated.h"

UCLASS()
class PLATFORMER_API UGameplayEventSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Event broadcasting
	UFUNCTION(BlueprintCallable, Category = "Events")
	void BroadcastEvent(const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable, Category = "Events")
	void BroadcastEventByID(const FString& EventID, AActor* Instigator = nullptr, const TMap<FString, FString>& Parameters = TMap<FString, FString>());

	// Event listening
	UFUNCTION(BlueprintCallable, Category = "Events")
	void RegisterEventListener(const FString& EventID, const FOnGameplayEvent& Delegate);

	UFUNCTION(BlueprintCallable, Category = "Events")
	void UnregisterEventListener(const FString& EventID, const FOnGameplayEvent& Delegate);

	// Global event delegate
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGameplayEvent OnAnyGameplayEvent;

protected:
	// Event registry
	UPROPERTY()
	TMap<FString, UGameplayEventAsset*> EventRegistry;

	// Event listeners
	TMap<FString, FOnGameplayEvent> EventListeners;

	// Event logging
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bLogAllEvents = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bShowAllEventsOnScreen = false;

public:
	// Registry management
	UFUNCTION(BlueprintCallable, Category = "Events")
	void RegisterEventAsset(UGameplayEventAsset* EventAsset);

	UFUNCTION(BlueprintCallable, Category = "Events")
	UGameplayEventAsset* GetEventAsset(const FString& EventID) const;

	UFUNCTION(BlueprintCallable, Category = "Events")
	TArray<FString> GetAllRegisteredEventIDs() const;

private:
	void LogEvent(const FGameplayEventData& EventData);
	void ShowEventOnScreen(const FGameplayEventData& EventData);
};
```

**Implementation `Source/Platformer/GameplayEventSubsystem.cpp`:**

```cpp
#include "GameplayEventSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

void UGameplayEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("GameplayEventSubsystem initialized"));
}

void UGameplayEventSubsystem::Deinitialize()
{
	EventListeners.Empty();
	EventRegistry.Empty();

	UE_LOG(LogTemp, Log, TEXT("GameplayEventSubsystem deinitialized"));

	Super::Deinitialize();
}

void UGameplayEventSubsystem::BroadcastEvent(const FGameplayEventData& EventData)
{
	// Set event time
	FGameplayEventData MutableEventData = EventData;
	MutableEventData.EventTime = GetWorld()->GetTimeSeconds();

	// Log the event
	if (bLogAllEvents)
	{
		LogEvent(MutableEventData);
	}

	// Show on screen if enabled
	if (bShowAllEventsOnScreen)
	{
		ShowEventOnScreen(MutableEventData);
	}

	// Check for specific event asset configuration
	if (UGameplayEventAsset* EventAsset = GetEventAsset(EventData.EventID))
	{
		if (EventAsset->bLogToConsole)
		{
			LogEvent(MutableEventData);
		}

		if (EventAsset->bShowOnScreen)
		{
			ShowEventOnScreen(MutableEventData);
		}
	}

	// Broadcast to global listeners
	OnAnyGameplayEvent.Broadcast(MutableEventData);

	// Broadcast to specific event listeners
	if (FOnGameplayEvent* SpecificDelegate = EventListeners.Find(EventData.EventID))
	{
		SpecificDelegate->Broadcast(MutableEventData);
	}

	UE_LOG(LogTemp, Verbose, TEXT("GameplayEvent broadcast: %s"), *EventData.EventID);
}

void UGameplayEventSubsystem::BroadcastEventByID(const FString& EventID, AActor* Instigator, const TMap<FString, FString>& Parameters)
{
	FGameplayEventData EventData(EventID, Instigator);
	EventData.Parameters = Parameters;

	// Get additional info from registry if available
	if (UGameplayEventAsset* EventAsset = GetEventAsset(EventID))
	{
		EventData.EventName = EventAsset->EventName;
		EventData.EventDescription = EventAsset->EventDescription;
	}

	BroadcastEvent(EventData);
}

void UGameplayEventSubsystem::RegisterEventListener(const FString& EventID, const FOnGameplayEvent& Delegate)
{
	if (!EventListeners.Contains(EventID))
	{
		EventListeners.Add(EventID, FOnGameplayEvent());
	}

	EventListeners[EventID].AddUObject(Delegate.GetUObject(), Delegate.GetFunctionName());
}

void UGameplayEventSubsystem::UnregisterEventListener(const FString& EventID, const FOnGameplayEvent& Delegate)
{
	if (FOnGameplayEvent* FoundDelegate = EventListeners.Find(EventID))
	{
		FoundDelegate->RemoveUObject(Delegate.GetUObject());
	}
}

void UGameplayEventSubsystem::RegisterEventAsset(UGameplayEventAsset* EventAsset)
{
	if (!EventAsset)
		return;

	EventRegistry.Add(EventAsset->EventID, EventAsset);

	UE_LOG(LogTemp, Log, TEXT("Registered event asset: %s"), *EventAsset->EventID);
}

UGameplayEventAsset* UGameplayEventSubsystem::GetEventAsset(const FString& EventID) const
{
	if (UGameplayEventAsset* const* FoundAsset = EventRegistry.Find(EventID))
	{
		return *FoundAsset;
	}
	return nullptr;
}

TArray<FString> UGameplayEventSubsystem::GetAllRegisteredEventIDs() const
{
	TArray<FString> EventIDs;
	EventRegistry.GetKeys(EventIDs);
	return EventIDs;
}

void UGameplayEventSubsystem::LogEvent(const FGameplayEventData& EventData)
{
	FString ParametersString;
	for (const auto& Param : EventData.Parameters)
	{
		ParametersString += FString::Printf(TEXT("%s=%s "), *Param.Key, *Param.Value);
	}

	UE_LOG(LogTemp, Log, TEXT("GameplayEvent [%s]: %s | Instigator: %s | Params: %s"),
		*EventData.EventID,
		*EventData.EventName.ToString(),
		EventData.Instigator ? *EventData.Instigator->GetName() : TEXT("None"),
		*ParametersString);
}

void UGameplayEventSubsystem::ShowEventOnScreen(const FGameplayEventData& EventData)
{
	if (GEngine)
	{
		FString DisplayText = FString::Printf(TEXT("Event: %s"), *EventData.EventName.ToString());

		float Duration = 3.0f;
		if (UGameplayEventAsset* EventAsset = GetEventAsset(EventData.EventID))
		{
			Duration = EventAsset->OnScreenDuration;
		}

		GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Cyan, DisplayText);
	}
}
```

### Step 4.7: Create ATriggerEventActor (60 minutes)

**Create Trigger Actor:**

Create `Source/Platformer/TriggerEventActor.h`:

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEvents.h"
#include "TriggerEventActor.generated.h"

class UBoxComponent;
class USphereComponent;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class ETriggerType : uint8
{
	OnBeginOverlap,
	OnEndOverlap,
	OnInteract,
	OnTimerBased
};

UENUM(BlueprintType)
enum class ETriggerShape : uint8
{
	Box,
	Sphere,
	Capsule
};

UCLASS()
class PLATFORMER_API ATriggerEventActor : public AActor
{
	GENERATED_BODY()

public:
	ATriggerEventActor();

protected:
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Trigger Configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	ETriggerType TriggerType = ETriggerType::OnBeginOverlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	ETriggerShape TriggerShape = ETriggerShape::Box;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	FVector TriggerSize = FVector(100, 100, 100);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	bool bTriggerOnce = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	bool bRequirePlayerOnly = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	float TriggerDelay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	float TimerInterval = 5.0f; // For timer-based triggers

	// Event Configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FString EventID = TEXT("TriggerEvent");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FText EventName = FText::FromString(TEXT("Trigger Event"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	FText EventDescription = FText::FromString(TEXT(""));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	TMap<FString, FString> EventParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	bool bLogEvent = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	bool bShowEventOnScreen = false;

	// State
	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bHasTriggered = false;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	TArray<AActor*> ActorsInTrigger;

public:
	// Trigger functions
	UFUNCTION(BlueprintCallable, Category = "Trigger")
	void FireEvent(AActor* Instigator = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Trigger")
	void ResetTrigger();

	UFUNCTION(BlueprintCallable, Category = "Trigger")
	bool CanTrigger(AActor* Instigator) const;

protected:
	// Overlap events
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Blueprint events
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnTriggered(AActor* Instigator);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnTriggerReset();

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void HandleTriggerActivation(AActor* Instigator);
	virtual void HandleTriggerActivation_Implementation(AActor* Instigator);

private:
	void SetupCollisionComponent();
	void UpdateCollisionShape();
	bool IsValidTriggerActor(AActor* Actor) const;

	FTimerHandle TriggerDelayTimerHandle;
	FTimerHandle TimerBasedTriggerHandle;
};
```

**Implementation `Source/Platformer/TriggerEventActor.cpp`:**

```cpp
#include "TriggerEventActor.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameplayEventSubsystem.h"
#include "PlatformCharacter.h"

ATriggerEventActor::ATriggerEventActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	// Create collision components
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetBoxExtent(TriggerSize);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetSphereRadius(TriggerSize.X);

	// Create visual mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Setup initial collision
	SetupCollisionComponent();
}

void ATriggerEventActor::BeginPlay()
{
	Super::BeginPlay();

	UpdateCollisionShape();

	// Setup timer-based triggering if needed
	if (TriggerType == ETriggerType::OnTimerBased && TimerInterval > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerBasedTriggerHandle,
			[this]()
			{
				if (CanTrigger(nullptr))
				{
					FireEvent(nullptr);
				}
			},
			TimerInterval,
			true // Loop
		);
	}

	UE_LOG(LogTemp, Log, TEXT("TriggerEventActor %s initialized with event ID: %s"),
		*GetName(), *EventID);
}

void ATriggerEventActor::SetupCollisionComponent()
{
	// Bind overlap events for both collision components
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ATriggerEventActor::OnBeginOverlap);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ATriggerEventActor::OnEndOverlap);

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ATriggerEventActor::OnBeginOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ATriggerEventActor::OnEndOverlap);

	// Configure collision responses
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ATriggerEventActor::UpdateCollisionShape()
{
	// Enable/disable collision components based on shape
	switch (TriggerShape)
	{
	case ETriggerShape::Box:
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoxCollision->SetBoxExtent(TriggerSize);
		break;

	case ETriggerShape::Sphere:
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereCollision->SetSphereRadius(TriggerSize.X);
		break;

	default:
		break;
	}
}

void ATriggerEventActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidTriggerActor(OtherActor))
		return;

	ActorsInTrigger.AddUnique(OtherActor);

	if (TriggerType == ETriggerType::OnBeginOverlap && CanTrigger(OtherActor))
	{
		if (TriggerDelay > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(
				TriggerDelayTimerHandle,
				[this, OtherActor]()
				{
					FireEvent(OtherActor);
				},
				TriggerDelay,
				false
			);
		}
		else
		{
			FireEvent(OtherActor);
		}
	}

	UE_LOG(LogTemp, Verbose, TEXT("TriggerEventActor %s: Actor entered - %s"),
		*GetName(), *OtherActor->GetName());
}

void ATriggerEventActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ActorsInTrigger.Remove(OtherActor);

	if (TriggerType == ETriggerType::OnEndOverlap && CanTrigger(OtherActor))
	{
		FireEvent(OtherActor);
	}

	UE_LOG(LogTemp, Verbose, TEXT("TriggerEventActor %s: Actor exited - %s"),
		*GetName(), *OtherActor->GetName());
}

bool ATriggerEventActor::IsValidTriggerActor(AActor* Actor) const
{
	if (!Actor)
		return false;

	if (bRequirePlayerOnly)
	{
		return Cast<APlatformCharacter>(Actor) != nullptr;
	}

	return true;
}

bool ATriggerEventActor::CanTrigger(AActor* Instigator) const
{
	if (bTriggerOnce && bHasTriggered)
		return false;

	if (bRequirePlayerOnly && Instigator && !Cast<APlatformCharacter>(Instigator))
		return false;

	return true;
}

void ATriggerEventActor::FireEvent(AActor* Instigator)
{
	if (!CanTrigger(Instigator))
		return;

	// Mark as triggered
	bHasTriggered = true;

	// Create event data
	FGameplayEventData EventData(EventID, Instigator);
	EventData.EventName = EventName;
	EventData.EventDescription = EventDescription;
	EventData.Parameters = EventParameters;
	EventData.EventLocation = GetActorLocation();

	// Add trigger-specific parameters
	EventData.Parameters.Add(TEXT("TriggerActor"), GetName());
	EventData.Parameters.Add(TEXT("TriggerType"), UEnum::GetValueAsString(TriggerType));

	// Broadcast event via subsystem
	if (UGameplayEventSubsystem* EventSubsystem = GetWorld()->GetSubsystem<UGameplayEventSubsystem>())
	{
		EventSubsystem->BroadcastEvent(EventData);
	}

	// Handle trigger activation
	HandleTriggerActivation(Instigator);

	// Call Blueprint event
	OnTriggered(Instigator);

	UE_LOG(LogTemp, Log, TEXT("TriggerEventActor %s fired event: %s"),
		*GetName(), *EventID);
}

void ATriggerEventActor::ResetTrigger()
{
	bHasTriggered = false;
	ActorsInTrigger.Empty();

	// Clear any pending timers
	GetWorld()->GetTimerManager().ClearTimer(TriggerDelayTimerHandle);

	OnTriggerReset();

	UE_LOG(LogTemp, Log, TEXT("TriggerEventActor %s reset"), *GetName());
}

void ATriggerEventActor::HandleTriggerActivation_Implementation(AActor* Instigator)
{
	// Default implementation - can be overridden in Blueprint or derived classes

	// Example: Change mesh color to indicate activation
	if (MeshComponent)
	{
		// This would need a dynamic material instance to work properly
		// For now, just log
		UE_LOG(LogTemp, Log, TEXT("Trigger activated - visual feedback would go here"));
	}
}
```

### Step 4.8: Create Example Interactive Events (45 minutes)

**Create Event Assets:**

1. Create several `UGameplayEventAsset` assets:
   - `GE_PlayerEnteredArea` (Log: true, Screen: true)
   - `GE_QuestTrigger` (Log: true, Screen: false)
   - `GE_StoryMoment` (Log: true, Screen: true, Duration: 5.0)

**Create Trigger Examples:**

1. **Area Trigger Blueprint:**
   - Create `BP_AreaTrigger` based on `ATriggerEventActor`
   - Set Event ID to "PlayerEnteredArea"
   - Configure as Box trigger, player-only, trigger once
   - Add descriptive mesh (invisible box or zone indicator)

2. **Story Moment Trigger:**
   - Create `BP_StoryTrigger` based on `ATriggerEventActor`
   - Set Event ID to "StoryMoment"
   - Add parameters like "ChapterID", "MomentName"
   - Configure with appropriate trigger settings

### Step 4.9: Create Test Level Setup (30 minutes)

**Level Configuration:**

1. Place `BP_FriendlyNPC` (based on `AFriendlyNPC`) in level
2. Configure NPC with multiple dialog sessions
3. Place several trigger areas around the level
4. Add patrol points for enemies from Milestone 3
5. Test integration between all systems

**NPC Dialog Configuration:**

```
Dialog Session 1: "Greeting"
- Line 1: "Hello there, traveler! Welcome to our town."
- Line 2: "I'm the local guide. I can tell you about this place."

Dialog Session 2: "Information"
- Line 1: "This area has some dangerous creatures at night."
- Line 2: "Be careful when exploring the darker areas."

Dialog Session 3: "Farewell"
- Line 1: "Safe travels! Come back if you need more help."
```

### Step 4.10: Final Integration & Testing (30 minutes)

**Test Comprehensive System:**

**Dialog System:**

- [ ] Player can talk to NPC with interaction prompt ✅
- [ ] Dialog UI displays correctly with speaker name and text ✅
- [ ] Multiple dialog sessions progress correctly ✅
- [ ] Dialog can be advanced with Space/Enter/E keys ✅
- [ ] Dialog ends properly and returns to gameplay ✅

**Event System:**

- [ ] Trigger areas fire events when player enters ✅
- [ ] Events are logged to console with correct data ✅
- [ ] Event parameters are passed correctly ✅
- [ ] One-time triggers work as expected ✅
- [ ] Timer-based triggers function properly ✅

**Integration:**

- [ ] NPCs and triggers work alongside Milestone 3 enemies ✅
- [ ] Performance remains stable with all systems active ✅
- [ ] UI systems don't conflict (health, interaction, dialog) ✅
- [ ] No errors in Output Log during testing ✅

## Acceptance Criteria Verification

**Final Testing Checklist:**

- [ ] Talk works with friendly NPCs ✅
- [ ] Dialog system shows proper UI with multiple lines ✅
- [ ] NPCs can have multiple dialog sessions ✅
- [ ] Trigger areas fire events when entered ✅
- [ ] Events are logged with parameters ✅
- [ ] Event subsystem routes events properly ✅
- [ ] All systems work together without conflicts ✅
- [ ] Performance is stable with all features active ✅

## Common Issues & Solutions

**Dialog doesn't start:**

- Check if NPC implements ITalkable correctly
- Verify dialog sessions are configured in NPC
- Ensure DialogWidget class is set on character

**Events don't fire:**

- Check trigger collision setup and responses
- Verify EventSubsystem is working (check logs)
- Ensure trigger shape and size are appropriate

**UI conflicts:**

- Check Z-order of widgets (dialog should be highest)
- Verify widget visibility states
- Test input handling priority

## Next Steps

Once Milestone 4 is complete:

1. Commit your code to Git
2. Test with different dialog configurations and event types
3. Consider adding branching dialog options
4. Begin [Milestone 5: Testing + Polish](/guide/milestone-5-detailed)

**Git Commit Message Example:**

```
Milestone 4: Implement friendly NPCs with dialog and event system

- Create ITalkable interface for conversation management
- Build AFriendlyNPC with multi-session dialog support
- Add comprehensive dialog UI with keyboard navigation
- Implement UGameplayEventSubsystem for world event routing
- Create ATriggerEventActor with multiple trigger types and shapes
- Integrate all systems with existing interaction and health UI
- Add event logging, parameters, and on-screen display options
- Test dialog progression and event triggering in complete game loop
```
