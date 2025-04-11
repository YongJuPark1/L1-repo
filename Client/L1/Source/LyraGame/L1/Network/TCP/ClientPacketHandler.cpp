// Fill out your copyright notice in the Description page of Project Settings.


#include "L1/Network/TCP/ClientPacketHandler.h"
#include "Player/LyraPlayerState.h"
#include "L1/Game/L1PlayerState.h"
#include "L1/Game/L1GameInstance.h"
#include "LyraLogChannels.h"
#include "L1/Game/L1GameMode.h"

bool ClientPacketHandler::Handle_FResPing(TSharedPtr<class PacketSession>& session, const FResPing& pkt)
{
	FString DebugMessage = FString::Printf(TEXT("[FResPing] ResCode: %d"), pkt.result.resCode);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);

	return true;
}

bool ClientPacketHandler::Handle_FResLogin(TSharedPtr<class PacketSession>& session, const FResLogin& pkt)
{
	FString DebugMessage = FString::Printf(TEXT("[FResLogin] Channel: %d, Nick: %s"), pkt.channelId, *pkt.nick);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);

	if (auto* GameInstance = Cast<UL1GameInstance>(GWorld->GetGameInstance()))
	{
		GameInstance->SetNickname(pkt.nick);
		GameInstance->SetPlayerUSN(pkt.usn);
		GameInstance->OnLoginResponse.Broadcast(pkt);
	}
	return true;
}

bool ClientPacketHandler::Handle_FResMatching(TSharedPtr<class PacketSession>& session, const FResMatching& pkt)
{
	FString DebugMessage = FString::Printf(TEXT("[FResMatching] ResCode: %d"), pkt.result.resCode);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);

	if (auto* GameInstance = Cast<UL1GameInstance>(GWorld->GetGameInstance()))
	{
		GameInstance->OnMatchingResponse.Broadcast(pkt);
	}

	return true;
}

bool ClientPacketHandler::Handle_FResMatchingCancel(TSharedPtr<class PacketSession>& session, const FResMatchingCancel& pkt)
{
	FString DebugMessage = FString::Printf(TEXT("[FResMatchingCancel] ResCode: %d"), pkt.result.resCode);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);

	return true;
}

bool ClientPacketHandler::Handle_FResMatchingUserCount(TSharedPtr<class PacketSession>& session, const FResMatchingUserCount& pkt)
{
	FString DebugMessage = FString::Printf(TEXT("[FResMatchingUserCount] ResCode: %d, userCount : %d"), pkt.result.resCode, pkt.userCount);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);

	if (auto* GameInstance = Cast<UL1GameInstance>(GWorld->GetGameInstance()))
	{
		GameInstance->OnMatchingUserCountResponse.Broadcast(pkt);
	}

	return true;
}

bool ClientPacketHandler::Handle_FResMatchingDone(TSharedPtr<class PacketSession>& session, const FResMatchingDone& pkt)
{
	FString DebugMessage = FString::Printf(TEXT("[FResMatchingDone] ResCode: %d, ip : %s, port : %d , process_id : %d"), pkt.result.resCode, *pkt.ip, pkt.port, *pkt.key);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);

	if (auto* GameInstance = Cast<UL1GameInstance>(GWorld->GetGameInstance()))
	{
		GameInstance->OnMatchingDoneResponse.Broadcast(pkt);
	}

	return true;
}

bool ClientPacketHandler::Handle_FResIngameUserResult(TSharedPtr<class PacketSession>& session, const FResIngameUserResult& pkt)
{
	FString DebugMessage = FString::Printf(TEXT("[FResIngameUserResult] ResCode: %d"), pkt.result.resCode);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);
		
	if (auto* GameInstance = Cast<UL1GameInstance>(GWorld->GetGameInstance()))
	{
		//GameInstance->OnMatchingDoneResponse.Broadcast(pkt);
	}

	return true;
}

bool ClientPacketHandler::Handle_FResSaveIngameResult(TSharedPtr<class PacketSession>& session, const FResSaveIngameResult& pkt)
{
	FString DebugMessage = FString::Printf(TEXT("[FResSaveIngameResult] ResCode: %d"), pkt.result.resCode);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);

	
	UWorld* World = GWorld;
	if (World && World->IsNetMode(NM_DedicatedServer))
	{
		AL1GameMode* GameMode = Cast<AL1GameMode>(World->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->ShutdownStart();
		}
	}
	return true;
}

bool ClientPacketHandler::Handle_FResDedicateShutdown(TSharedPtr<class PacketSession>& session, const FResDedicateShutdown& pkt)
{
	UWorld* World = GWorld;
	if (World && World->IsNetMode(NM_DedicatedServer))
	{
		AL1GameMode* GameMode = Cast<AL1GameMode>(World->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->ShutdownEnd();
		}
	}

	return true;
}
