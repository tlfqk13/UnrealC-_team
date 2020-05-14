
#include "SurvivalGame.h"
#include "SCoopGameMode.h"
#include "NavigationSystem.h"
#include "SPlayerState.h"
#include "SCharacter.h"
#include "SGameState.h"

static FSocket* inst = nullptr;

FSocket& MySocket::getInstance() {
	return *inst;
}

void MySocket::initializeServer()
{
	inst = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("My_Server"), false);
	
	inst->SetNonBlocking();

	//FString ipaddress = TEXT("127.0.0.1");

	//FIPv4Address ip(127, 0, 0, 1);
	FIPv4Address ip;
	FIPv4Address::Parse(Adress, ip);

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	addr->SetIp(ip.Value);
	addr->SetPort(SERVERPORT);

	bool connected = inst->Connect(*addr);

	if (!connected) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Fail to Connect!! ")));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Success to Connect!! ")));
		Connected = true;

	}


	if (Connected)
	{
		S_Login Login;
		/*int32 zero = 0;
		bool Success = inst->Send((uint8*)&Login, (int32)sizeof(Login), zero);
		if (Success) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("S_Login Send! SIZE : %d"), (int32)sizeof(Login)));
		}
		MySocket::RecvPacket();*/
		MySocket::sendBuffer(PACKET_CS_LOGIN, &Login);
	}

}

void MySocket::sendBuffer(int PacketType, void* BUF) {
	int32 zero = 0;
	bool Success;
	
	switch (PacketType) {
	case PACKET_CS_LOGIN:
	{
		S_Login* packet = reinterpret_cast<S_Login*>(BUF);
		Success = inst->Send((uint8*)packet, (int32)sizeof(*packet), zero);
		if (Success) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("S_Login Send! SIZE : %d"), (int32)sizeof(*packet)));
		}
	}
	break;
	case PACKET_CS_GAME_START:
	{
		S_Start* packet = reinterpret_cast<S_Start*>(BUF);
		Success = inst->Send((uint8*)packet, (int32)sizeof(*packet), zero);
		if (Success) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("S_Start Send! SIZE : %d"), (int32)sizeof(*packet)));
		}
	}
	break;
	case PACKET_CS_LOCATION: 
	{
		S_Loc* packet = reinterpret_cast<S_Loc*>(BUF);
		Success = inst->Send((uint8*)packet, (int32)sizeof(*packet), zero);
		if (Success) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("s_obj SIZE : %d"), (int32)sizeof(*tmp)));
			/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player X : %f, Y : %f, Z : %f "),
				packet->clientLoc.x, packet->clientLoc.y, packet->clientLoc.z));*/
		}
	}
	break;
	case PACKET_CS_JUMP:
	{	
		S_Jump* packet = reinterpret_cast<S_Jump*>(BUF);
		Success = inst->Send((uint8*)packet, (int32)sizeof(*packet), zero);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("JUMP!")));
	}
	break;
	}		
	
	MySocket::RecvPacket();
}

void MySocket::RecvPacket() {
	uint8* RECV_BUF = new uint8[MAX_BUFFER];
	R_Test* p_tmp;
	int32 zero = 0;

	bool Success = inst->Recv(RECV_BUF, (int32)MAX_BUFFER, zero);

	if (!Success) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Fail to Recv!! ")));
	}
	else {

		p_tmp = reinterpret_cast<R_Test*>(RECV_BUF);

		switch (p_tmp->packet_type) 
		{
			case PACKET_SC_LOGIN:
			{
				Playing = 0;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Success to Recv LOGIN!! ")));
				R_Login* packet = reinterpret_cast<R_Login*>(RECV_BUF);
				if(PlayerId == -1)
					PlayerId = packet->clientId;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("PlayerId : %d "),
					PlayerId));
				if (HostPlayer == -1)
					HostPlayer = packet->Host;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Host Player : %d "),
					HostPlayer));
				for (int i = 0; i < MAX_USER; ++i) {
					Player_info.IsUsed[i] = packet->Player[i];
					if (Player_info.IsUsed[i])
						Playing++;
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d Player Login : %d "),
						i + 1,packet->Player[i]));
				}
			}
			break;
			case PACKET_SC_GAME_START:
			{
				R_Start* packet = reinterpret_cast<R_Start*>(RECV_BUF);
				GameStart = packet->Started;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Started : %d "),GameStart));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("GAME START! ")));
			}
			break;
			case PACKET_SC_PLAYERS:
			{
				R_Players* packet = reinterpret_cast<R_Players*>(RECV_BUF);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Players RECV! ")));
				for (int i = 0; i < MAX_USER; ++i) 
				{
					Player_info.IsUsed[i] = packet->IsUsed[i];
					Player_info.Loc[i] = packet->Loc[i];
					Player_info.IsJump[i] = packet->IsJump[i];
				}
				
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("X : %f, Y : %f, Z: %f "),
					Player_info.Loc[PlayerId].x, Player_info.Loc[PlayerId].y, Player_info.Loc[PlayerId].z));
			}
			break;
		}
		
	}

	delete[] RECV_BUF;
}

void ASCoopGameMode::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();  // Before widget delete
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport(); // View current widget
		}
	}
}

void ASCoopGameMode::SetIpAdress(FString Ip_Adress)
{
	Adress = Ip_Adress;
}

bool ASCoopGameMode::ConnectServer()
{
	MySocket::initializeServer();
	if (Connected)
		return true;
	else
		return false;
}

bool ASCoopGameMode::IsConected()
{
	if (Connected)
		return true;
	else
		return false;
}


int ASCoopGameMode::GetPlayer()
{
	return Playing;
}

bool ASCoopGameMode::IsHost()
{
	if (PlayerId == HostPlayer)
		return true;
	else
		return false;
}

bool ASCoopGameMode::IsStart()
{
	if (GameStart)
		return true;
	else
		return false;
}

void ASCoopGameMode::Start()
{
	S_Start start;
	MySocket::sendBuffer(PACKET_CS_GAME_START, &start);
}

void ASCoopGameMode::StartPlay()
{
	Super::StartPlay();

	PlayerId = -1;
	HostPlayer = -1;
	Connected = false;
	GameStart = false;

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World!!"));
		
		ChangeMenuWidget(StartingWidgetClass);  // View main widget
	}

	

}

ASCoopGameMode::ASCoopGameMode(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* Disable damage to coop buddies  */
	bAllowFriendlyFireDamage = false;
	bSpawnAtTeamPlayer = true;
	
	ScoreNightSurvived = 1000;
}


/*
	RestartPlayer - Spawn the player next to his living coop buddy instead of a PlayerStart
*/
void ASCoopGameMode::RestartPlayer(class AController* NewPlayer)
{
	//if (Connected)
	{

		/* Fallback to PlayerStart picking if team spawning is disabled or we're trying to spawn a bot. */
		if (!bSpawnAtTeamPlayer || (NewPlayer->PlayerState && NewPlayer->PlayerState->bIsABot))
		{
			Super::RestartPlayer(NewPlayer);
			return;
		}

		/* Look for a live player to spawn next to */
		FVector SpawnOrigin = FVector::ZeroVector;
		FRotator StartRotation = FRotator::ZeroRotator;
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
		{
			ASCharacter* MyCharacter = Cast<ASCharacter>(*It);
			if (MyCharacter && MyCharacter->IsAlive())
			{
				/* Get the origin of the first player we can find */
				SpawnOrigin = MyCharacter->GetActorLocation();
				StartRotation = MyCharacter->GetActorRotation();
				break;
			}
		}

		/* No player is alive (yet) - spawn using one of the PlayerStarts */
		if (SpawnOrigin == FVector::ZeroVector)
		{
			Super::RestartPlayer(NewPlayer);
			return;
		}

		/* Get a point on the nav mesh near the other player */
		FNavLocation StartLocation;
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(this);
		if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(SpawnOrigin, 250.0f, StartLocation))
		{
			// Try to create a pawn to use of the default class for this player
			if (NewPlayer->GetPawn() == nullptr && GetDefaultPawnClassForController(NewPlayer) != nullptr)
			{
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.Instigator = Instigator;
				APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(GetDefaultPawnClassForController(NewPlayer), StartLocation.Location, StartRotation, SpawnInfo);
				if (ResultPawn == nullptr)
				{
					UE_LOG(LogGameMode, Warning, TEXT("Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(DefaultPawnClass), &StartLocation.Location);
				}
				NewPlayer->SetPawn(ResultPawn);
			}

			if (NewPlayer->GetPawn() == nullptr)
			{
				NewPlayer->FailedToSpawnPawn();
			}
			else
			{
				NewPlayer->Possess(NewPlayer->GetPawn());

				// If the Pawn is destroyed as part of possession we have to abort
				if (NewPlayer->GetPawn() == nullptr)
				{
					NewPlayer->FailedToSpawnPawn();
				}
				else
				{
					// Set initial control rotation to player start's rotation
					NewPlayer->ClientSetRotation(NewPlayer->GetPawn()->GetActorRotation(), true);

					FRotator NewControllerRot = StartRotation;
					NewControllerRot.Roll = 0.f;
					NewPlayer->SetControlRotation(NewControllerRot);

					SetPlayerDefaults(NewPlayer->GetPawn());
				}
			}
		}
	}
}


void ASCoopGameMode::OnNightEnded()
{
	/* Respawn spectating players that died during the night */
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		/* Look for all players that are spectating */
		ASPlayerController* MyController = Cast<ASPlayerController>(*It);
		if (MyController)
		{
			if (MyController->PlayerState->bIsSpectator)
			{
				RestartPlayer(MyController);
				MyController->ClientHUDStateChanged(EHUDState::Playing);
			}
			else
			{
				/* Player still alive, award him some points */
				ASCharacter* MyPawn = Cast<ASCharacter>(MyController->GetPawn());
				if (MyPawn && MyPawn->IsAlive())
				{
					ASPlayerState* PS = Cast<ASPlayerState>(MyController->PlayerState);
					if (PS)
					{
						PS->ScorePoints(ScoreNightSurvived);
					}
				}
			}
		}
	}
}


void ASCoopGameMode::Killed(AController* Killer, AController* VictimPlayer, APawn* VictimPawn, const UDamageType* DamageType)
{
	ASPlayerState* KillerPS = Killer ? Cast<ASPlayerState>(Killer->PlayerState) : NULL;
	ASPlayerState* VictimPS = VictimPlayer ? Cast<ASPlayerState>(VictimPlayer->PlayerState) : NULL;

	if (KillerPS && KillerPS != VictimPS && !KillerPS->bIsABot)
	{
		KillerPS->AddKill();
		KillerPS->ScorePoints(10);
	}

	if (VictimPS && !VictimPS->bIsABot)
	{
		VictimPS->AddDeath();
	}

	/* End match is all players died */
	CheckMatchEnd();
}


void ASCoopGameMode::CheckMatchEnd()
{
	bool bHasAlivePlayer = false;
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		ASCharacter* MyPawn = Cast<ASCharacter>(*It);
		if (MyPawn && MyPawn->IsAlive())
		{
			ASPlayerState* PS = Cast<ASPlayerState>(MyPawn->GetPlayerState());
			if (PS)
			{
				if (!PS->bIsABot)
				{
					/* Found one player that is still alive, game will continue */
					bHasAlivePlayer = true;
					break;
				}
			}
		}
	}

	/* End game is all players died */
	if (!bHasAlivePlayer)
	{
		FinishMatch();
	}
}


void ASCoopGameMode::FinishMatch()
{
	ASGameState* const MyGameState = Cast<ASGameState>(GameState);
	if (IsMatchInProgress())
	{
		EndMatch();

		/* Stop spawning bots */
		GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawns);

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
		{
			ASPlayerController* MyController = Cast<ASPlayerController>(*It);
			if (MyController)
			{
				MyController->ClientHUDStateChanged(EHUDState::MatchEnd);
			}
		}
	}
}
