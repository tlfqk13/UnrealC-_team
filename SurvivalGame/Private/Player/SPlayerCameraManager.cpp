

#include "SurvivalGame.h"
#include "SPlayerCameraManager.h"
#include "SCharacter.h"


ASPlayerCameraManager::ASPlayerCameraManager(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NormalFOV = 90.0f;
	TargetingFOV = 65.0f;

	ViewPitchMin = -80.0f;
	ViewPitchMax = 87.0f;
	bAlwaysApplyModifiers = true;

	/* The camera delta between standing and crouched */
	MaxCrouchOffsetZ = 35.0f;

	/* HACK. Mirrored from the 3rd person camera offset from SCharacter */
	DefaultCameraOffsetZ = 20.0f;

	/* Ideally matches the transition speed of the character animation (crouch to stand and vice versa) */
	CrouchLerpVelocity = 12.0f;
}


void ASPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
}


void ASPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	ASCharacter* MyPawn = PCOwner ? Cast<ASCharacter>(PCOwner->GetPawn()) : nullptr;
	if (MyPawn)
	{
		const float TargetFOV = MyPawn->IsTargeting() ? TargetingFOV : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV, TargetFOV, DeltaTime, 20.0f);
		SetFOV(DefaultFOV);
	}

	
	if (MyPawn)
	{
		if (MyPawn->bIsCrouched && !bWasCrouched)
		{
			CurrentCrouchOffset = MaxCrouchOffsetZ;
		}
		else if (!MyPawn->bIsCrouched && bWasCrouched)
		{
			CurrentCrouchOffset = -MaxCrouchOffsetZ;
		}

		bWasCrouched = MyPawn->bIsCrouched;
	
		CurrentCrouchOffset = FMath::Lerp(CurrentCrouchOffset, 0.0f, FMath::Clamp(CrouchLerpVelocity * DeltaTime, 0.0f, 1.0f));

		FVector CurrentCameraOffset = MyPawn->GetCameraComponent()->GetRelativeTransform().GetLocation();
		FVector NewCameraOffset = FVector(CurrentCameraOffset.X, CurrentCameraOffset.Y, DefaultCameraOffsetZ + CurrentCrouchOffset);
		MyPawn->GetCameraComponent()->SetRelativeLocation(NewCameraOffset);
	}

	Super::UpdateCamera(DeltaTime);
}