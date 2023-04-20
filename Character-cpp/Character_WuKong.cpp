// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_WuKong.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACharacter_WuKong::ACharacter_WuKong()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Creating basic components used by this character
	PrimaryActorTick.bCanEverTick = true;
	CharacterMesh = GetMesh();

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Arm Component"));
	SpringArm->SetupAttachment(CharacterMesh);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetWorldLocation(FVector(0.0f, 0.0f, 180.0f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Character Camera"));
	CameraComponent->SetupAttachment(SpringArm);

	widgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	widgetComponent->SetupAttachment(CharacterMesh);

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System"));
	ParticleSystemComponent->SetupAttachment(CharacterMesh);

	RangedParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Ranged Particle System"));
	RangedParticleSystemComponent->SetupAttachment(CharacterMesh);
}

// Called when the game starts or when spawned
void ACharacter_WuKong::BeginPlay()
{
	Super::BeginPlay();

	//Saving reference of commonly used component.
	CharacterMovement = GetCharacterMovement();
	AnimInstance = GetMesh()->GetAnimInstance();

	//Getting reference to component which was not created using the script.
	// Finding weapon component, if found adding a OnComponentBeginOverlap event function
	// else, showing an error as log
	for (int32 i = 0; i < CharacterMesh->GetNumChildrenComponents(); i++)
	{
		USceneComponent* ChildComponent = CharacterMesh->GetChildComponent(i);
		if (ChildComponent->ComponentHasTag("Weapon"))
		{
			WeaponComponent = Cast<UCapsuleComponent>(ChildComponent);
			break;
		}
	}

	if (WeaponComponent != NULL)
	{
		WeaponComponent->OnComponentBeginOverlap.AddDynamic(this, &ACharacter_WuKong::CollisionFunction);
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponComponent not Found"));
	}

	// Adding value to variable at start of this character
	health = 100;
	mana = 100;
	bIsDead = false;
	initialWalkSpeed = CharacterMovement->MaxWalkSpeed;

	// Casting and saving references from Widget component
	HealthBar = Cast<UProgressBar>(widgetComponent->GetUserWidgetObject()->WidgetTree->FindWidget(TEXT("HealthBar")));
	ManaBar = Cast<UProgressBar>(widgetComponent->GetUserWidgetObject()->WidgetTree->FindWidget(TEXT("ManaBar")));
	
	// if this character is controlled by player controller then activate the character,
	if(IsPlayerControlled())
		bHasActivated = true;		
}

// Called every frame
void ACharacter_WuKong::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If any attack montage is playing, make bIsArracking true
	bIsAttacking = AnimInstance->Montage_IsPlaying(MeleeMontage);

	//Reset function of double jump
	if (!CharacterMovement->IsFalling())
		bDoDouble = true;
	
	// Character is doin the ranged attack when its doing its animation or its FX is active.
	bIsDoingRangedMove = RangedParticleSystemComponent->IsActive() || AnimInstance->Montage_IsPlaying(RangedMoveMontage);
}

// Called to bind functionality to input
void ACharacter_WuKong::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Attacking PlayerInput
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACharacter_WuKong::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACharacter_WuKong::MoveSideWard);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ACharacter_WuKong::LookSide);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ACharacter_WuKong::LookUp);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter_WuKong::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ACharacter_WuKong::Attack);
	PlayerInputComponent->BindAction(TEXT("Regenerate"), EInputEvent::IE_Pressed, this, &ACharacter_WuKong::Regenerate);
	PlayerInputComponent->BindAction(TEXT("Dodge"), EInputEvent::IE_Pressed, this, &ACharacter_WuKong::Dodge);
}

// Make Character move forward respective of camera.
void ACharacter_WuKong::MoveForward(float value)
{
	AddMovementInput(CameraComponent->GetForwardVector(), value);
}

// Make character move sides respective of camera.
void ACharacter_WuKong::MoveSideWard(float value)
{
	AddMovementInput(CameraComponent->GetRightVector(), value);
}

//Make Character look left and right
void ACharacter_WuKong::LookSide(float value)
{
	AddControllerYawInput(value);
}

//Make character look up or down 
void ACharacter_WuKong::LookUp(float value)
{
	AddControllerPitchInput(value);
}

//Make Character Jump
void ACharacter_WuKong::Jump()
{
	Super::Jump();

	//if the player presses the button within a time, do double Jump
	float WaitTime5 = 0.5f;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle5, [this]()
	{
			// This function will be called after the wait time has elapsed
			bDoDouble = false;
	}, WaitTime5, false);

	//if character is in air and player pressed the button within time
	// do a double jump, and add a double jump animation and a FX
	if (CharacterMovement->IsFalling() && bDoDouble)
	{
		bDoDouble = false;
		FVector JumpImpulse = GetActorUpVector() * (DodgeImpulse / 2);
		CharacterMovement->AddImpulse(JumpImpulse, true);
		ParticleSystemComponent->SetTemplate(DoubleJumpFX);
		PlayAnimMontage(JumpStartMontage);
		ParticleSystemComponent->Activate();
	}
}

// Making character attack enemies
void ACharacter_WuKong::Attack()
{
	// If character is not already attacking and not taking hits from enemies then do attack animation
	if (!AnimInstance->Montage_IsPlaying(MeleeMontage) && !AnimInstance->Montage_IsPlaying(DamageMontage) &&
		!AnimInstance->Montage_IsPlaying(RangedMoveMontage) && !RangedParticleSystemComponent->IsActive())
	{
		PlayAnimMontage(MeleeMontage);
	}
}

// Function for regenerating health of character
void ACharacter_WuKong::Regenerate()
{
	// if character's health is not full and he has mana to spare then 
	// do the FX, calculate health and mana and update the UI widget
	if (health < 100 && mana > 0)
	{
		ParticleSystemComponent->SetTemplate(RegenerateFX);
		ParticleSystemComponent->Activate();
		int healthRequired = 100 - health;
		if (mana >= healthRequired)
		{
			health = 100;
			mana = mana - healthRequired;
			UpdateBars();
		}
		else
		{
			health = health + mana;
			mana = 0;
			UpdateBars();
		}
	}

}

// Function of character dodging
void ACharacter_WuKong::Dodge()
{
	// Play Dodge animation and do a backward impulse
	PlayAnimMontage(DodgeMontage);
	FVector ImpulseVector = ( - GetActorForwardVector() * DodgeImpulse) + (GetActorUpVector() * (DodgeImpulse / 2));
	CharacterMovement->AddImpulse(ImpulseVector, true);
}

//Function for updating widget
void ACharacter_WuKong::UpdateBars()
{
	float percent1 = health / 100.0f;
	float percent2 = mana / 100.0f;
	HealthBar->SetPercent(percent1);
	ManaBar->SetPercent(percent2);
}

// Function for taking damage
void ACharacter_WuKong::DecreaseHealth(int damage)
{
	// if the character is not activated, and taken damage then kill instantly (Stealth Kill)
	// Decrease health, reset mana increase, play a taking hit animation, and check whether the character is dead
	// if dead, then call CharacterDead() function,
	// else, update widget, start timing for recovering from the hit
	if (!bHasActivated)
	{
		damage = 100;
	}
	health -= damage;
	ManaIncrease = 0;
	PlayAnimMontage(DamageMontage);
	if (health <= 0)
		CharacterDead();
	UpdateBars();
	bIsCooling = true;

	float WaitTime = 1.5f;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle3, [this]()
		{
			// This function will be called after the wait time has elapsed
			bIsCooling = false;
		}, WaitTime, false);
}

// Function of doing Character Dead
void ACharacter_WuKong::CharacterDead()
{
	//Play Death Animation, set bIsDead to true, and destroy the component after some time.
	PlayAnimMontage(DeathMontage);
	bIsDead = true;

	float WaitTime2 = 1.2f;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle4, [this]()
	{
			CharacterMesh->DestroyComponent();
			this->Destroy();
	}, WaitTime2, false);
}

// Function for sensing danger
void ACharacter_WuKong::DangerSense()
{
	// Plays FX, 
	// The time of whole game set to be slow motion so that the character can dodge
	// the time is reseted to normal after some time
	ParticleSystemComponent->SetTemplate(DangerFX);
	ParticleSystemComponent->Activate();
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.375f);
	float WaitTime7 = 0.5f;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle7, [this]()
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

		}, WaitTime7, false);
}

// Function to Increase Mana
void ACharacter_WuKong::IncreaseMana(int value)
{
	// if mana is not full them, increase mana but not more than 100
	// the amount of increase gradually so, after some seconds reset it.
	if (mana < 100)
	{
		ManaIncrease++;
		mana += value;
		mana = FMath::Clamp(mana, 0, 100);
		UpdateBars();
	}

	float WaitTime6 = 5.0f;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle6, [this]()
		{
			ManaIncrease = 0;
		}, WaitTime6, false);
}

// FUnction for moving a ranged attack
void ACharacter_WuKong::DoRangedMove()
{
	// Play animation of the move and FX after certain seconds,
	// Making sure the animation is already is playing
	if (!bTimeractivated)
	{
		CharacterMovement->StopActiveMovement();
		PlayAnimMontage(RangedMoveMontage);
		bTimeractivated = true;
		float WaitTime8 = 4.0f;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle8, [this]()
			{
				RangedParticleSystemComponent->SetTemplate(RangeMoveFX);
				RangedParticleSystemComponent->Activate();
				bTimeractivated = false;
			}, WaitTime8, false);
	}
	
}

// Function to decrease the speed of this character
void ACharacter_WuKong::DecreaseSpeed()
{
	CharacterMovement->MaxWalkSpeed = initialWalkSpeed/2;
}

// Function to resetting the speed of the character
void ACharacter_WuKong::ResetSpeed()
{
	CharacterMovement->MaxWalkSpeed = initialWalkSpeed;
}

// Function called when weapon started collided with object
void ACharacter_WuKong::CollisionFunction(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	// Making sure if it hit other actor and we are attacking, not just a random collision
	// Getting the script of the other actor, then
	// we call functions of the actor to decreasing its health, also increases mana of ours
	if (HitComp != NULL && OtherActor != NULL && OtherComp != NULL)
	{
		if (OtherActor != this && AnimInstance->Montage_IsPlaying(MeleeMontage) && !bIsHitted && !bIsCooling)
		{

			ACharacter_WuKong* script = Cast<ACharacter_WuKong>(OtherActor);
			if ( script &&!script->bIsDead)
			{
				script->DecreaseHealth(10);
				float WaitTime = 1.0f;
				bIsHitted = true;
				IncreaseMana(ManaIncrease);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle2, [this]()
					{
						bIsHitted = false;
					}, WaitTime, false);
			}
		}
	}
}

// Function return if the character is doing the rangedMove or not
bool ACharacter_WuKong::GetbIsDoingRangedMove()
{
	return bIsDoingRangedMove;
}
