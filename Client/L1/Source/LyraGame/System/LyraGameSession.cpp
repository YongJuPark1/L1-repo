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
					// FInGameUserInfo 가져오기
					const FInGameUserInfo* UserInfo = PlayerState->GetIngameUserInfo();

					// 로그 출력
					UE_LOG(LogLyraExperience, Log, TEXT("Player End Game Info - Nick: %s, USN: %d, TeamId: %d"),
						*UserInfo->Nick, UserInfo->Usn, UserInfo->TeamId);
					UE_LOG(LogLyraExperience, Log, TEXT("GamePlay Stats - Kills: %d, Deaths: %d, Assists: %d"),
						UserInfo->ingamePlayInfo.killCnt, UserInfo->ingamePlayInfo.deathCnt, UserInfo->ingamePlayInfo.assistCnt);
				}
			}
		}
	}

	Super::HandleMatchHasEnded();
}

