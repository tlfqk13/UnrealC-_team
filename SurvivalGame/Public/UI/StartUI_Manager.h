// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SCoopGameMode.h"
#include "GameFramework/Actor.h"
#include "StartUI_Manager.generated.h"

UCLASS()
class SURVIVALGAME_API AStartUI_Manager : public AActor
{
	GENERATED_BODY()
		bool Recving = false;
public:	
	// Sets default values for this actor's properties
	AStartUI_Manager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
