// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraGameSession.h"
#include "Engine/World.h"
#include "Net/OnlineEngineInterface.h"
#include "L1/Game/L1PlayerState.h"
#include "LyraLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraGameSession)


ALyraGameSession::ALyraGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool ALyraGameSession::ProcessAutoLogin()
{
	// This is actually handled in LyraGameMode::TryDedicatedServerLogin
	return true;
}

void ALyraGameSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void ALyraGameSession::HandleMatchHasEnded()
{
	UWorld* World = GetWorld();
	if (UOnlineEngineInterface::Get()->DoesSessionExist(World, SessionName))
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			
			if (PlayerController)
			{
				AL1PlayerState* PlayerState = PlayerController->GetPlayerState<AL1PlayerState>();

				if (PlayerState)
				{
					if (!PlayerState->GetIngameUserInfo().IsValid())
						continue;
					
					// 로그 출력
					UE_LOG(LogLyraExperience, Log, TEXT("Player End Game Info - Nick: %s, USN: %d, TeamId: %d"),
						*PlayerState->GetIngameUserInfo()->Nick, PlayerState->GetIngameUserInfo()->Usn, PlayerState->GetIngameUserInfo()->TeamId);
					UE_LOG(LogLyraExperience, Log, TEXT("GamePlay Stats - Kills: %d, Deaths: %d, Assists: %d"),
						PlayerState->GetIngameUserInfo()->ingamePlayInfo.killCnt, PlayerState->GetIngameUserInfo()->ingamePlayInfo.deathCnt, PlayerState->GetIngameUserInfo()->ingamePlayInfo.assistCnt);
				}
			}
		}
	}

	Super::HandleMatchHasEnded();
}

