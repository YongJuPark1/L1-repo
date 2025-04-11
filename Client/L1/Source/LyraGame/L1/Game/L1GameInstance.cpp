// Fill out your copyright notice in the Description page of Project Settings.

#include "L1/Game/L1GameInstance.h"
#include "L1/Network/TCP/PacketSession.h"
#include "L1/Network/TCP/TCPRequest.h"
#include "msgpack/msgpack.hpp"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Common/TcpSocketBuilder.h"
#include "LyraLogChannels.h"
#include "Player/LyraPlayerState.h"



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

	if (GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		return;
	}

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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("DedicateServer To GameServer Connecting To Server...")));
	UE_LOG(LogLyraExperience, Log, TEXT("********************DedicateServer To GameServer Connected Server Connecting To Server********************"));

	isConnect = Socket->Connect(*InternetAddr);

	if (isConnect)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("DedicateServer To GameServer Connected Success")));
		UE_LOG(LogLyraExperience, Log, TEXT("********************DedicateServer To GameServer Connected Server Connect Success********************"));
		GameServerSession = MakeShared<PacketSession>(Socket);
		GameServerSession->Run();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("DedicateServer To GameServer Connected Falied")));
		UE_LOG(LogLyraExperience, Log, TEXT("********************DedicateServer To GameServer Connected Server Connect Falied********************"));
	}

}

void UL1GameInstance::Shutdown()
{
	UE_LOG(LogLyraExperience, Log, TEXT("********************DedicateServer Shutdown********************"));
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
	UE_LOG(LogLyraExperience, Log, TEXT("********************Client To GameServer Connected Server Connecting To Server********************"));

	isConnect = Socket->Connect(*InternetAddr);

	if (isConnect)
	{	
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connected Success")));
		UE_LOG(LogLyraExperience, Log, TEXT("********************Client To GameServer Connected Success********************"));
		GameServerSession = MakeShared<PacketSession>(Socket);
		GameServerSession->Run();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connected Falied")));
		UE_LOG(LogLyraExperience, Log, TEXT("********************Client To GameServer Connected Falied********************"));
	}

	OnConnectGameServer.Broadcast(isConnect);
}

void UL1GameInstance::DisconnectFromGameServer()
{
	if (GameServerSession)
	{
		GameServerSession->Disconnect();
		GameServerSession = nullptr;
	}

	if (Socket)
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("DisConnected Success")));
	}

	isConnect = false;
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

void UL1GameInstance::SendMatching(int32 _matchingType)
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	ReqMatching packet;
	packet.matchingType = _matchingType;
	SEND_PACKET(packet);
}

void UL1GameInstance::SendMatchingCancel()
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}

	ReqMatchingCancel packet;
	SEND_PACKET(packet);
}

void UL1GameInstance::SendMatchingDone()
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}
}

void UL1GameInstance::SendMatchingResult(FString ip, int32 port, int32 idx, int32 matchingType, FString ingameResult)
{	
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}
	ReqSaveIngameResult packet;
	packet.ip = TCHAR_TO_UTF8(*ip);
	packet.port = port;
	packet.idx = idx;
	packet.matchingType = matchingType;
	packet.ingameResult = TCHAR_TO_UTF8(*ingameResult);
	SEND_PACKET(packet);
	
}

void UL1GameInstance::SendShutdownDedicateServer(int32 dediIdx)
{
	UE_LOG(LogTemp, Log, TEXT("Shutdown TimerStart - 5"));
	ReqDedicateShutdown packet;
	packet.dediIdx = dediIdx;
	SEND_PACKET(packet);
}

void UL1GameInstance::LoginProcess()
{
	if (Socket == nullptr || GameServerSession == nullptr)
	{
		return;
	}
	ReqLogin packet;
	packet.platformType = 1;
	packet.token = TCHAR_TO_UTF8(*strUSN);
	SEND_PACKET(packet);
}

void UL1GameInstance::SetUSN(const FText& USN)
{
	strUSN = USN.ToString();


}

void UL1GameInstance::SetNickname(const FString& Nick)
{
	strNick = Nick;
}




