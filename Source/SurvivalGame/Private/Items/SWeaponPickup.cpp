

#include "SurvivalGame.h"
#include "SCharacter.h"
#include "SWeapon.h"
#include "SWeaponPickup.h"
#include "SPlayerController.h"


ASWeaponPickup::ASWeaponPickup(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bAllowRespawn = false;

	
	bReplicateMovement = true;
}


void ASWeaponPickup::OnUsed(APawn* InstigatorPawn)
{
	ASCharacter* MyPawn = Cast<ASCharacter>(InstigatorPawn);
	if (MyPawn)
	{
		
		if (MyPawn->WeaponSlotAvailable(WeaponClass->GetDefaultObject<ASWeapon>()->GetStorageSlot()))
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ASWeapon* NewWeapon = GetWorld()->SpawnActor<ASWeapon>(WeaponClass, SpawnInfo);

			MyPawn->AddWeapon(NewWeapon);

			Super::OnUsed(InstigatorPawn);
		}
		else
		{
			ASPlayerController* PC = Cast<ASPlayerController>(MyPawn->GetController());
			if (PC)
			{
				PC->ClientHUDMessage(EHUDMessage::Weapon_SlotTaken);
			}
		}
	}
}


