

#pragma once

#include "SBaseCharacter.h"
#include "SZombieCharacter.generated.h"

UCLASS(ABSTRACT)
class SURVIVALGAME_API ASZombieCharacter : public ASBaseCharacter
{
	GENERATED_BODY()

	/* Last time the player was spotted */
	float LastSeenTime;

	/* Last time the player was heard */
	float LastHeardTime;

	/* Last time we attacked something */
	float LastMeleeAttackTime;

	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SenseTimeOut;

	
	bool bSensedTarget;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	class UPawnSensingComponent* PawnSensingComp;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

protected:

	virtual bool IsSprinting() const override;

	UFUNCTION()
	void OnSeePlayer(APawn* Pawn);
	
	UFUNCTION()
	void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

	UPROPERTY(VisibleAnywhere, Category = "Attacking")
	UCapsuleComponent* MeleeCollisionComp;

	/* A pawn is in melee range */
	UFUNCTION()
	void OnMeleeCompBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	void OnRetriggerMeleeStrike();

	/* Deal damage to the Actor that was hit by the punch animation */
	UFUNCTION(BlueprintCallable, Category = "Attacking")
	void PerformMeleeStrike(AActor* HitActor);

	UFUNCTION(Reliable, NetMulticast)
	void SimulateMeleeStrike();

	void SimulateMeleeStrike_Implementation();

	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
	TSubclassOf<UDamageType> PunchDamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
	float MeleeDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Attacking")
	UAnimMontage* MeleeAnimMontage;

	/* Update the vocal loop of the zombie (idle, wandering, hunting) */
	UFUNCTION(Reliable, NetMulticast)
	void BroadcastUpdateAudioLoop(bool bNewSensedTarget);

	void BroadcastUpdateAudioLoop_Implementation(bool bNewSensedTarget);

	UAudioComponent* PlayCharacterSound(USoundCue* CueToPlay);

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundPlayerNoticed;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundHunting;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundIdle;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundWandering;

	UPROPERTY(EditDefaultsOnly, Category = "Sound") 
	USoundCue* SoundAttackMelee;

	
	FTimerHandle TimerHandle_MeleeAttack;

	/* Minimum time between melee attacks */
	float MeleeStrikeCooldown;

	/* Plays the idle, wandering or hunting sound */
	UPROPERTY(VisibleAnywhere, Category = "Sound")
	UAudioComponent* AudioLoopComp;

	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled) override;

public:

	ASZombieCharacter(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, Category = "Attacking")
	bool bIsPunching;
	
	
	UPROPERTY(EditAnywhere, Category = "AI")
	EBotBehaviorType BotType;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	
	void SetBotType(EBotBehaviorType NewType);
};
