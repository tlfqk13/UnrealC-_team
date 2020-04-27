

#include "SurvivalGame.h"
#include "SCharacterMovementComponent.h"
#include "SBaseCharacter.h"



float USCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const ASBaseCharacter* CharOwner = Cast<ASBaseCharacter>(PawnOwner);
	if (CharOwner)
	{
		
		if (CharOwner->IsTargeting() && !IsCrouching())
		{
			MaxSpeed *= CharOwner->GetTargetingSpeedModifier();
		}
		else if (CharOwner->IsSprinting())
		{
			MaxSpeed *= CharOwner->GetSprintingSpeedModifier();
		}
	}

	return MaxSpeed;
}