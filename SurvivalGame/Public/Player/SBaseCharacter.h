

#pragma once

#include "GameFramework/Character.h"
#include "STypes.h"
#include "SBaseCharacter.generated.h"

UCLASS(ABSTRACT)
class SURVIVALGAME_API ASBaseCharacter : public ACharacter
{
	GENERATED_BODY()

	/* Tracks noise data used by the pawn sensing component */
	UPawnNoiseEmitterComponent* NoiseEmitterComp;

public:

	ASBaseCharacter(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundTakeHit;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundDeath;

	/************************************************************************/
	/* Health                                                               */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual bool IsSprinting() const;


	virtual void SetSprinting(bool NewSprinting);

	float GetSprintingSpeedModifier() const;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintingSpeedModifier;

	UPROPERTY(Transient, Replicated)
	bool bWantsToRun;

	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSprinting(bool NewSprinting);

	void ServerSetSprinting_Implementation(bool NewSprinting);

	bool ServerSetSprinting_Validate(bool NewSprinting);

	/************************************************************************/
	/* Targeting                                                            */
	/************************************************************************/

	void SetTargeting(bool NewTargeting);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetTargeting(bool NewTargeting);

	void ServerSetTargeting_Implementation(bool NewTargeting);

	bool ServerSetTargeting_Validate(bool NewTargeting);

	UPROPERTY(Transient, Replicated)
	bool bIsTargeting;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingSpeedModifier;

public:

	
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	bool IsTargeting() const;

	float GetTargetingSpeedModifier() const;

	
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	FRotator GetAimOffsets() const;

	/************************************************************************/
	/* Damage & Death                                                       */
	/************************************************************************/

protected:

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition", Replicated)
	float Health;

	
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);

	virtual void FellOutOfWorld(const class UDamageType& DmgType) override;

	void SetRagdollPhysics();

	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	void ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	UFUNCTION()
	void OnRep_LastTakeHitInfo();

	bool bIsDying;
};
