// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SurvivalGame.h"
#include "CoreMinimal.h"
#include "Engine/Classes/Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "SCoopGameMode.h"
#include "SCharacter.h"
#include "SPlayerController.h"
#include "SPlayerCameraManager.h"
#include "SLocalPlayer.h"
#include "Player_Manager.generated.h"


UCLASS()
class SURVIVALGAME_API APlayer_Manager : public AActor
{
	GENERATED_BODY()

	ASCharacter* Array_Player[MAX_USER] = { NULL };

	bool test = false;

public:	
	// Sets default values for this actor's properties
	APlayer_Manager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	ASCharacter** Get_Player_Array();

	template<class T>
	T* SpawnActor( UClass* Class, FVector const& Location, FRotator const& Rotation )
	{
		AActor* Owner = NULL;
		APawn* Instigator = NULL;
		bool bNoCollisionFail = false;
		return (Class != NULL) ? Cast<T>(GetWorld()->SpawnActor(Class, &Location, &Rotation)) : NULL;
	}

	ASCharacter* actorspawn(FVector Loc, FRotator Rot);
	void CheckNewPlayer();
};

