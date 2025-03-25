// Fill out your copyright notice in the Description page of Project Settings.

#include "L1/Game/L1GameInstance.h"
#include "L1/Network/TCP/PacketSession.h"
#include "L1/Network/TCP/TCPRequest.h"
#include "msgpack/msgpack.hpp"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Common/TcpSocketBuilder.h"
#include "LyraLogChannels.h"



UL1GameInstance::UL1GameInstance(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	
}

void UL1GameInstance::Init()
{
	Super::Init();

	if (false == GetWorld()->GetTimerManager().IsTimerActive(PingTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(PingTimerHandle, this, &UL1GameInstance::SendPingCheck, 3.0f, true);
	}

	if (false == GetWorld()->GetTimerManager().IsTimerActive(RecvTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(RecvTimerHandle, this, &UL1GameInstance::HandleRecvPackets, PacketProcessInterval, true);
	}

}

void UL1GameInstance::Shutdown()
{
	Super::Shutdown();
}

void UL1GameInstance::ConnectToGameServer()
{
	if (GetWorld()->GetNetMode() == NM_DedicatedServer || GetWorld()->GetNetMode() == NM_ListenServer)
	{
		return;
	}

	//소켓이 있는데 Connect 할 경우
	if (Socket) 
	{
		DisconnectFromGameServer();
	}

	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

	FIPv4Address Ip = NULL;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedPtr<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connecting To Server...")));

	isConnect = Socket->Connect(*InternetAddr);

	if (isConnect)
	{	
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connected Success")));
		GameServerSession = MakeShared<PacketSession>(Socket);
		GameServerSession->Run();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connected Falied")));
	}

	OnConnectGameServer.Broadcast(isConnect);
}

void UL1GameInstance::DisconnectFromGameServer()
{
}

void UL1GameInstance::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;


	GameServerSession->HandleRecvPackets();
}

void UL1GameInstance::SendPacket(TSharedPtr<class SendBuffer> SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}



void UL1GameInstance::SendPingCheck()
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	ReqPing packet;
	SEND_PACKET(packet);
}

