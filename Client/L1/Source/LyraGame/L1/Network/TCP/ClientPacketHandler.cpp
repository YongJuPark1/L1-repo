// Fill out your copyright notice in the Description page of Project Settings.


#include "L1/Network/TCP/ClientPacketHandler.h"


bool ClientPacketHandler::Handle_FResPing(TSharedPtr<class PacketSession>& session, const FResPing& pkt)
{
	FString DebugMessage = FString::Printf(TEXT("[FResPing] ResCode: %d"), pkt.result.resCode);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);

	return true;
}
