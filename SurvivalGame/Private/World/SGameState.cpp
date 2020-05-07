

#include "SurvivalGame.h"
#include "SPlayerController.h"
#include "SGameState.h"


ASGameState::ASGameState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* 1 SECOND real time is 1*TimeScale MINUTES game time */
	TimeScale = 15.0f;
	bIsNight = false;

	SunriseTimeMark = 3.0f;
	SunsetTimeMark = 0.03f;
}


void ASGameState::SetTimeOfDay(float NewTimeOfDay)
{
	ElapsedGameMinutes = NewTimeOfDay;
}


bool ASGameState::GetIsNight()
{
	return bIsNight;
}


float ASGameState::GetTimeOfDayIncrement()
{
	return (GetWorldSettings()->GetEffectiveTimeDilation() * TimeScale);
}


int32 ASGameState::GetElapsedDays()
{
	const float MinutesInDay = 24 * 60;
	const float ElapsedDays = ElapsedGameMinutes / MinutesInDay;
	return FMath::FloorToInt(ElapsedDays);
}


int32 ASGameState::GetElapsedFullDaysInMinutes()
{
	const int32 MinutesInDay = 24 * 60;
	return GetElapsedDays() * MinutesInDay;
}


bool ASGameState::GetAndUpdateIsNight()
{
	const float TimeOfDay = ElapsedGameMinutes - GetElapsedFullDaysInMinutes();
	if (TimeOfDay > (SunriseTimeMark * 60) && TimeOfDay < (SunsetTimeMark * 60))
	{
		bIsNight = false;
	}
	else
	{
		bIsNight = true;
	}

	return bIsNight;
}


int32 ASGameState::GetRealSecondsTillSunrise()
{
	float SunRiseMinutes = (SunriseTimeMark * 60);
	const int32 MinutesInDay = 24 * 60;

	float ElapsedTimeToday = GetElapsedMinutesCurrentDay();
	if (ElapsedTimeToday < SunRiseMinutes)
	{
	
		return (SunRiseMinutes - ElapsedTimeToday) / TimeScale;
	}
	else
	{
		
		float MaxTimeTillNextSunrise = MinutesInDay + SunRiseMinutes;
		return (MaxTimeTillNextSunrise - ElapsedTimeToday) / TimeScale;
	}
}


int32 ASGameState::GetElapsedMinutesCurrentDay()
{
	return ElapsedGameMinutes - GetElapsedFullDaysInMinutes();
}


void ASGameState::BroadcastGameMessage_Implementation(EHUDMessage MessageID)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		ASPlayerController* MyController = Cast<ASPlayerController>(*It);
		if (MyController && MyController->IsLocalController())
		{
			MyController->ClientHUDMessage(MessageID);
		}
	}
}


int32 ASGameState::GetTotalScore()
{
	return TotalScore;
}


void ASGameState::AddScore(int32 Score)
{
	TotalScore += Score;
}


void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, ElapsedGameMinutes);
	DOREPLIFETIME(ASGameState, bIsNight);
	DOREPLIFETIME(ASGameState, TotalScore);
}