// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ProgressBar.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "CharacterWidget.h"
#include "GameFramework/Controller.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character_WuKong.generated.h"

UCLASS()
class SAMPEPROJECT_API ACharacter_WuKong : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacter_WuKong();

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* widgetComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	UCapsuleComponent* WeaponComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* MeleeMontage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* DamageMontage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* JumpStartMontage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Montages")
	UAnimMontage* RangedMoveMontage;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ParticleSystemComponent;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* RangedParticleSystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FXs")
	UParticleSystem* RegenerateFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FXs")
	UParticleSystem* DangerFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FXs")
	UParticleSystem* DoubleJumpFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FXs")
	UParticleSystem* RangeMoveFX;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	int health;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	int mana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Script_bool")
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Script_bool")
	bool bIsAttacking;


	bool bIsCooling;
	bool bIsHitted;
	bool bDoDouble = true;
	bool bHasActivated = false;

	FTimerHandle TimerHandle;
	FTimerHandle TimerHandle2;
	FTimerHandle TimerHandle3;
	FTimerHandle TimerHandle4;
	FTimerHandle TimerHandle5;
	FTimerHandle TimerHandle6;
	FTimerHandle TimerHandle7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeImpulse")
	float DodgeImpulse = 1000.0f;

	int ManaIncrease = 0;
	float initialWalkSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UMeshComponent* CharacterMesh;
	UCharacterMovementComponent* CharacterMovement;
	UAnimInstance* AnimInstance;
	UProgressBar* HealthBar;
	UProgressBar* ManaBar;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void MoveForward(float value);
	virtual void MoveSideWard(float value);
	virtual void LookUp(float value);
	virtual void LookSide(float value);
	virtual void Jump();
	virtual void Attack();
	virtual void Regenerate();
	virtual void Dodge();

	void UpdateBars();

	UFUNCTION()
	virtual void CollisionFunction(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);


	UFUNCTION()
	virtual void DecreaseHealth(int damage);

	UFUNCTION()
	virtual void CharacterDead();

	UFUNCTION()
	virtual void DangerSense();

	UFUNCTION()
	virtual void IncreaseMana(int value);

	UFUNCTION()
	void DoRangedMove();

	UFUNCTION()
	void DecreaseSpeed();

	UFUNCTION()
	void ResetSpeed();

};
