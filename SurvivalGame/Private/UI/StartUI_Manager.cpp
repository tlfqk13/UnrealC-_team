// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"

#include "StartUI_Manager.h"

// Sets default values
AStartUI_Manager::AStartUI_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStartUI_Manager::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickInterval(0.5f);
	
}

// Called every frame
void AStartUI_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Connected)
	{
		/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connected : %d "), Connected));
		S_Login Login;
		int32 zero = 0;
		bool Success = MySocket::getInstance().Send((uint8*)&Login, (int32)sizeof(Login), zero);
		if (Success) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("S_Login Send! SIZE : %d"), (int32)sizeof(Login)));
		}*/

		MySocket::RecvPacket();
		//Recving = true;
	}
}
	
