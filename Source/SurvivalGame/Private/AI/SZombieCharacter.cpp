

#include "SurvivalGame.h"
#include "SZombieCharacter.h"
#include "SZombieAIController.h"
#include "SCharacter.h"
#include "SBaseCharacter.h"
#include "SBotWaypoint.h"
#include "SPlayerState.h"

/* AI Include */
#include "Perception/PawnSensingComponent.h"

// Sets default values
ASZombieCharacter::ASZombieCharacter(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(60.0f);
	PawnSensingComp->SightRadius = 2000;
	PawnSensingComp->HearingThreshold = 600;
	PawnSensingComp->LOSHearingThreshold = 1200;

	
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f, false);
	GetCapsuleComponent()->SetCapsuleRadius(42.0f);

	
	GetMovementComponent()->NavAgentProps.AgentRadius = 42;
	GetMovementComponent()->NavAgentProps.AgentHeight = 192;

	MeleeCollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MeleeCollision"));
	MeleeCollisionComp->SetRelativeLocation(FVector(45, 0, 25));
	MeleeCollisionComp->SetCapsuleHalfHeight(60);
	MeleeCollisionComp->SetCapsuleRadius(35, false);
	MeleeCollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeleeCollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	MeleeCollisionComp->SetupAttachment(GetCapsuleComponent());

	AudioLoopComp = CreateDefaultSubobject<UAudioComponent>(TEXT("ZombieLoopedSoundComp"));
	AudioLoopComp->bAutoActivate = false;
	AudioLoopComp->bAutoDestroy = false;
	AudioLoopComp->SetupAttachment(RootComponent);

	Health = 100;
	MeleeDamage = 24.0f;
	MeleeStrikeCooldown = 1.0f;
	SprintingSpeedModifier = 3.0f;

	/* By default we will not let the AI patrol, we can override this value per-instance. */
	BotType = EBotBehaviorType::Passive;
	SenseTimeOut = 1.5f;

}


void ASZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* This is the earliest moment we can bind our delegates to the component */
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &ASZombieCharacter::OnSeePlayer);
		PawnSensingComp->OnHearNoise.AddDynamic(this, &ASZombieCharacter::OnHearNoise);
	}
	if (MeleeCollisionComp)
	{
		MeleeCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASZombieCharacter::OnMeleeCompBeginOverlap);
	}

	BroadcastUpdateAudioLoop(bSensedTarget);

	/* Assign a basic name to identify the bots in the HUD. */
	ASPlayerState* PS = Cast<ASPlayerState>(GetPlayerState());
	if (PS)
	{
		PS->SetPlayerName("Bot");
		PS->bIsABot = true;
	}
}


void ASZombieCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/* Check if the last time we sensed a player is beyond the time out value to prevent bot from endlessly following a player. */
	if (bSensedTarget && (GetWorld()->TimeSeconds - LastSeenTime) > SenseTimeOut 
		&& (GetWorld()->TimeSeconds - LastHeardTime) > SenseTimeOut)
	{
		ASZombieAIController* AIController = Cast<ASZombieAIController>(GetController());
		if (AIController)
		{
			bSensedTarget = false;
			/* Reset */
			AIController->SetTargetEnemy(nullptr);

			/* Stop playing the hunting sound */
			BroadcastUpdateAudioLoop(false);
		}
	}
}


void ASZombieCharacter::OnSeePlayer(APawn* Pawn)
{
	if (!IsAlive())
	{
		return;
	}

	if (!bSensedTarget)
	{
		BroadcastUpdateAudioLoop(true);
	}

	/* Keep track of the time the player was last sensed in order to clear the target */
	LastSeenTime = GetWorld()->GetTimeSeconds();
	bSensedTarget = true;

	ASZombieAIController* AIController = Cast<ASZombieAIController>(GetController());
	ASBaseCharacter* SensedPawn = Cast<ASBaseCharacter>(Pawn);
	if (AIController && SensedPawn->IsAlive())
	{
		AIController->SetTargetEnemy(SensedPawn);
	}
}


void ASZombieCharacter::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	if (!IsAlive())
	{
		return;
	}

	if (!bSensedTarget)
	{
		BroadcastUpdateAudioLoop(true);
	}

	bSensedTarget = true;
	LastHeardTime = GetWorld()->GetTimeSeconds();

	ASZombieAIController* AIController = Cast<ASZombieAIController>(GetController());
	if (AIController)
	{
		AIController->SetTargetEnemy(PawnInstigator);
	}
}


void ASZombieCharacter::PerformMeleeStrike(AActor* HitActor)
{
	if (LastMeleeAttackTime > GetWorld()->GetTimeSeconds() - MeleeStrikeCooldown)
	{
		/* Set timer to start attacking as soon as the cooldown elapses. */
		if (!TimerHandle_MeleeAttack.IsValid())
		{
			// TODO: Set Timer
		}

		/* Attacked before cooldown expired */
		return;
	}

	if (HitActor && HitActor != this && IsAlive())
	{
		ACharacter* OtherPawn = Cast<ACharacter>(HitActor);
		if (OtherPawn)
		{
			ASPlayerState* MyPS = Cast<ASPlayerState>(GetPlayerState());
			ASPlayerState* OtherPS = Cast<ASPlayerState>(OtherPawn->GetPlayerState());

			if (MyPS && OtherPS)
			{
				if (MyPS->GetTeamNumber() == OtherPS->GetTeamNumber())
				{
					/* Do not attack other zombies. */
					return;
				}

				/* Set to prevent a zombie to attack multiple times in a very short time */
				LastMeleeAttackTime = GetWorld()->GetTimeSeconds();

				FPointDamageEvent DmgEvent;
				DmgEvent.DamageTypeClass = PunchDamageType;
				DmgEvent.Damage = MeleeDamage;

				HitActor->TakeDamage(DmgEvent.Damage, DmgEvent, GetController(), this);

				SimulateMeleeStrike();
			}
		}
	}
}


void ASZombieCharacter::SetBotType(EBotBehaviorType NewType)
{
	BotType = NewType;
	
	ASZombieAIController* AIController = Cast<ASZombieAIController>(GetController());
	if (AIController)
	{
		AIController->SetBlackboardBotType(NewType);
	}

	BroadcastUpdateAudioLoop(bSensedTarget);
}


UAudioComponent* ASZombieCharacter::PlayCharacterSound(USoundCue* CueToPlay)
{
	if (CueToPlay)
	{
		return UGameplayStatics::SpawnSoundAttached(CueToPlay, RootComponent, NAME_None, FVector::ZeroVector, EAttachLocation::SnapToTarget, true);
	}

	return nullptr;
}


void ASZombieCharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled)
{
	Super::PlayHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, bKilled);

	/* Stop playing the hunting sound */
	if (AudioLoopComp && bKilled)
	{
		AudioLoopComp->Stop();
	}
}


void ASZombieCharacter::SimulateMeleeStrike_Implementation()
{
	PlayAnimMontage(MeleeAnimMontage);
	PlayCharacterSound(SoundAttackMelee);
}


void ASZombieCharacter::OnMeleeCompBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	/* Stop any running attack timers */
	TimerHandle_MeleeAttack.Invalidate();

	PerformMeleeStrike(OtherActor);
	
	/* Set re-trigger timer to re-check overlapping pawns at melee attack rate interval */
	GetWorldTimerManager().SetTimer(TimerHandle_MeleeAttack, this, &ASZombieCharacter::OnRetriggerMeleeStrike, MeleeStrikeCooldown, true);
}


void ASZombieCharacter::OnRetriggerMeleeStrike()
{
	/* Apply damage to a single random pawn in range. */
	TArray<AActor*> Overlaps;
	MeleeCollisionComp->GetOverlappingActors(Overlaps, ASBaseCharacter::StaticClass());
	for (int32 i = 0; i < Overlaps.Num(); i++)
	{
		ASBaseCharacter* OverlappingPawn = Cast<ASBaseCharacter>(Overlaps[i]);
		if (OverlappingPawn)
		{
			PerformMeleeStrike(OverlappingPawn);
			
		}
	}

	
	if (Overlaps.Num() == 0)
	{
		TimerHandle_MeleeAttack.Invalidate();
	}
}


bool ASZombieCharacter::IsSprinting() const
{
	/* Allow a zombie to sprint when he has seen a player */
	return bSensedTarget && !GetVelocity().IsZero();
}


void ASZombieCharacter::BroadcastUpdateAudioLoop_Implementation(bool bNewSensedTarget)
{
	/* Start playing the hunting sound and the "noticed player" sound if the state is about to change */
	if (bNewSensedTarget && !bSensedTarget)
	{
		PlayCharacterSound(SoundPlayerNoticed);

		AudioLoopComp->SetSound(SoundHunting);
		AudioLoopComp->Play();
	}
	else
	{
		if (BotType == EBotBehaviorType::Patrolling)
		{
			AudioLoopComp->SetSound(SoundWandering);
			AudioLoopComp->Play();
		}
		else
		{
			AudioLoopComp->SetSound(SoundIdle);
			AudioLoopComp->Play();
		}
	}
}