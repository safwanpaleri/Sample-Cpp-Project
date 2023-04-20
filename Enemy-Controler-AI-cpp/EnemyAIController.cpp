// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Character_WuKong.h"
#include "GameFramework/Actor.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	//Wait 1 second for initiating everything
	float WaitTime2 = 1.0f;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, [this]()
	{
		// Getting reference to player character.
		// Saving reference of the pawn in which this script is connected.
		// setting bIsAwake = true, so that functions in Update Works
		// Saving reference of the character script to which this AI controller is attached to.
		playerCharacter = Cast<ACharacter_WuKong>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		thisPawn = GetPawn();
		bIsAwake = true;
		thisCharacter = Cast<ACharacter_WuKong>(thisPawn);
		bIsTimer3Started = false;
	}, WaitTime2, false);

	
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick((DeltaTime));

	// if 1 second after starting the level is not done, return
	if (!bIsAwake)
		return;

	//if this actor is not activated ( or if this actor has not seen the player character)
	if (!thisCharacter->bHasActivated)
	{
		//Check's if the player character is in front of this actor
		// if true it activates this actor
		// Also disables Stealth kill option
		if (CheckIfInfrontOfActor())
		{
			thisCharacter->bHasActivated = true;
		}
	}

	// If we got every references successfully and the actor got activated
	if (thisPawn && playerCharacter && thisCharacter->bHasActivated)
	{
		//Getting location of this actor and player character
		//Comparing the distance between them and doing accordingly
		myLocation = thisPawn->GetActorLocation();
		playerLocation = playerCharacter->GetActorLocation();
		float Distance = FVector::Distance(myLocation, playerLocation);

		//if the character is not within the actor attack reach
		// Move to player of use a Ranged Attack
		// else if the character is within the reach of actor's attack range then attack
		if (!IsWithinRange(myLocation, playerLocation, 200.0f))
		{
			int random = FMath::RandRange(0, 10);
			if (random > 8 && !bCheckPos && !bIsMoving)
			{
				thisCharacter->GetCharacterMovement()->StopMovementImmediately();
				RangedAttack();
				
			}
			else
			{
				// if the character is not doing a ranged attack, then move to player
				if (!thisCharacter->GetbIsDoingRangedMove())
				{
					MoveTo(playerLocation);
					bIsMoving = true;
				}
			}
			if(!bCheckPos)
			{
				float WaitTime4 = 1.0f;
				bCheckPos = true;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle4, [this]()
					{
						bCheckPos = false;
						bIsMoving = false;;
						playerCharacter->ResetSpeed();
					}, WaitTime4, false);
			}
				

		}
		else
		{
			
			MeleeAttack();
			bIsMoving = false;
			bCheckPos = false;
		}
		
	}
	
	// if the enemy AI's health is less than 20, regenerate health
	if (thisCharacter->health < 20)
		thisCharacter->Regenerate();
	
	// if doing ranged attack then call its function
	if (bRangedAttack)
	{
		RangedAttack();
	}
}

// Compares two location and returns if the within a certain range
bool AEnemyAIController::IsWithinRange(const FVector& LocationA, const FVector& LocationB, float rangeDistance)
{
	float Distance = FVector::Distance(LocationA, LocationB);
	return Distance < rangeDistance;
}


//Functionality to do a ranged Attack
void AEnemyAIController::RangedAttack()
{
	// If it is currently not doing a ranged Attack and player is not dead, then
	// Do the ranged move, also decrease the speed so that player does not escape easily
	// else if the attack is already on, them 
	// decrease the health after every half second, also increase the magnitude of damage taken by the player
	// if the player also managed to escape the range of ranged attack, reset the speed of character and reset magnitude of damage
	if (!thisCharacter->GetbIsDoingRangedMove() && !playerCharacter->bIsDead)
	{
		thisCharacter->DoRangedMove();
		playerCharacter->DecreaseSpeed();
		bRangedAttack = true;
	}
	else
	{
		if (IsWithinRange(myLocation, playerLocation, 800.0f) && !playerCharacter->bIsDead)
		{
			if (!bIsTimer3Started && thisCharacter->RangedParticleSystemComponent->IsActive())
			{
				float WaitTime3 = 0.5f;
				bIsTimer3Started = true;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle3, [this]()
					{
						RangedDamage += 5;
						playerCharacter->DecreaseHealth(RangedDamage);
						bIsTimer3Started = false;
						UE_LOG(LogTemp, Warning, TEXT("Decrease Health"));

					}, WaitTime3, false);
			}
			bRangedAttack = true;
		}
		else
		{
			RangedDamage = 0;
			playerCharacter->ResetSpeed();
			bRangedAttack = false;
		}
	}
}

//Function for doing a meleeAttack
void AEnemyAIController::MeleeAttack()
{
	// If every references is correct and this actor or player character is not dead
	// also if this actor is not attacking and not hurt after a hit then,
	// play Attack and wait for some seconds before next attack,
	if (thisCharacter && !playerCharacter->bIsDead && !thisCharacter->bIsDead)
	{
		if (bHasWaited && !thisCharacter->bIsAttacking && !thisCharacter->bIsCooling)
		{

			
			bHasWaited = false;

			playerCharacter->DangerSense();

			thisCharacter->Attack();

			float WaitTime2 = 2.5f;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle2, [this]()
				{
					bHasWaited = true;

				}, WaitTime2, false);


		}
	}
}

//Functionality checks if the player character is in front of this actor.
bool AEnemyAIController::CheckIfInfrontOfActor()
{
	ActorToPlayer = playerCharacter->GetActorLocation() - thisCharacter->GetActorLocation();
	ActorToPlayer.Normalize();

	DotProduct = FVector::DotProduct(thisCharacter->GetActorForwardVector(), ActorToPlayer);

	if (DotProduct > 0.5f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

