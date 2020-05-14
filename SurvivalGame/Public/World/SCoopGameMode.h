

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"
#include "Blueprint/UserWidget.h"
#include "SGameMode.h"
#include "SCoopGameMode.generated.h"

#define MAX_USER 4
#define SERVERPORT 9000
#define MAX_BUFFER 1024

#define PACKET_CS_GAME_START 300
#define PACKET_SC_GAME_START 301

#define PACKET_SC_LOGIN 100
#define PACKET_SC_LOCATION 101
#define PACKET_SC_JUMP 102
#define PACKET_SC_PLAYERS 103

#define PACKET_CS_LOGIN 200
#define PACKET_CS_LOCATION 201
#define PACKET_CS_JUMP 202

typedef struct LOCATION {
	float x;
	float y;
	float z;
}Location;

typedef struct Info_Player{
	bool IsUsed[MAX_USER] = { false };
	Location Loc[MAX_USER];
	bool IsJump[MAX_USER] = { false };
}Player;

typedef struct RECVOBJECT {
	int clientId;
	bool isUsed[MAX_USER] = { false };
	Location clientLoc[MAX_USER];
}R_Obj;

typedef struct SENDOBJECT {
	int clientId;
	bool isUsed[MAX_USER] = { false };
	Location clientLoc;
	//bool keyBuffer[MAX_BUFFER];
}S_Obj;

typedef struct Send_Packet {
	int packet_type;
	void* Buffer;
}S_packet;

typedef struct Test_Packet {
	int packet_type;
	int i;
}R_Test;

typedef struct Send_Packet_Location {
	int packet_type = PACKET_CS_LOCATION;
	Location clientLoc;
}S_Loc;

typedef struct Send_Packet_Jump {
	int packet_type = PACKET_CS_JUMP;
	int i;
}S_Jump;

typedef struct Send_Packet_Login {
	int packet_type = PACKET_CS_LOGIN;
}S_Login;

typedef struct Recv_Packet_Login {
	int packet_type = PACKET_SC_LOGIN;
	int clientId;
	bool Player[MAX_USER] = { false };
	int Host;
}R_Login;

typedef struct Send_Packet_GAME_START {
	int packet_type = PACKET_CS_GAME_START;
}S_Start;

typedef struct Recv_Packet_GAME_START {
	int packet_type = PACKET_SC_GAME_START;
	bool Started;
}R_Start;


typedef struct Recv_Packet_Players {
	int packet_type = PACKET_SC_PLAYERS;
	bool IsUsed[MAX_USER];
	Location Loc[MAX_USER];
	bool IsJump[MAX_USER];
}R_Players;


static Player Player_info;
static int PlayerId;
static int Playing;
static FString Adress;
static bool Connected;
static int HostPlayer;
static bool GameStart;

class MySocket {
private:
	MySocket() {}
public:
	static FSocket& getInstance();

	static void initializeServer();
	static void sendBuffer(int PacketType, void* BUF);
	static void RecvPacket();
};


UCLASS(ABSTRACT)
class SURVIVALGAME_API ASCoopGameMode : public ASGameMode
{

	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UMG Game")
		void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass); // Widget change function

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		void SetIpAdress(FString Ip_Adress);

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		bool ConnectServer();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		bool IsConected();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		int GetPlayer();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		bool IsHost();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		bool IsStart();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		void Start();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> StartingWidgetClass; // Main widget

	UPROPERTY()
		UUserWidget* CurrentWidget; // Output widget

public:

	virtual void StartPlay() override;

	ASCoopGameMode(const FObjectInitializer& ObjectInitializer);

	/* End the match when all players are dead */
	void CheckMatchEnd();

	/* End the match, with a delay before returning to the main menu */
	void FinishMatch();

	/* Spawn the player next to his living coop buddy instead of a PlayerStart */
	virtual void RestartPlayer(class AController* NewPlayer) override;

	virtual void OnNightEnded() override;

	/* Spawn at team player if any are alive */
	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	bool bSpawnAtTeamPlayer;

	virtual void Killed(AController* Killer, AController* VictimPlayer, APawn* VictimPawn, const UDamageType* DamageType) override;

	/************************************************************************/
	/* Scoring                                                              */
	/************************************************************************/

	/* Points awarded for surviving a night */
	UPROPERTY(EditDefaultsOnly, Category = "Scoring")
	int32 ScoreNightSurvived;

};
