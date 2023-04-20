// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Character_WuKong.h"
#include "GameFramework/Character.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SAMPEPROJECT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(VisibleAnywhere)
	class ACharacter_WuKong* playerCharacter;
	class ACharacter_WuKong* thisCharacter;

	APawn* thisPawn;

	FVector myLocation;
	FVector playerLocation;
	FVector ActorToPlayer;

	FTimerHandle TimerHandle1;
	FTimerHandle TimerHandle2;
	FTimerHandle TimerHandle3;
	FTimerHandle TimerHandle4;

	bool bIsAwake = false;
	bool bIsTimer3Started = false;
	bool bHasWaited = true;
	bool bIsMoving = false;
	bool bCheckPos = false;
	bool bRangedAttack = false;

	int RangedDamage = 0;
	float DotProduct;

	UFUNCTION()
	void RangedAttack();

	UFUNCTION()
	void MeleeAttack();

	UFUNCTION()
	bool CheckIfInfrontOfActor();

	UFUNCTION()
	bool IsWithinRange(const FVector& LocationA, const FVector& LocationB, float rangeDistance);

};
