// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/LyraGameInstance.h"
#include "TimerManager.h"
#include "L1/Network/TCP/TCPResponse.h"
#include "L1/Network/TCP/ClientPacketHandler.h"
#include "L1/Network/TCP/PacketSession.h"
#include "L1GameInstance.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConnectGameServer, bool, isConnect);


#define SEND_PACKET(Pkt)														\
	TSharedPtr<class SendBuffer> SendBuffer = ClientPacketHandler::MakeSendBuffer(Pkt);		\
	Cast<UL1GameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);

UCLASS()
class LYRAGAME_API UL1GameInstance : public ULyraGameInstance
{
	GENERATED_BODY()

public:
	UL1GameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 10501;
	TSharedPtr<class PacketSession> GameServerSession;

protected:
	virtual void Init() override;
	virtual void Shutdown() override;

public:
	void SendPacket(TSharedPtr<class SendBuffer> SendBuffer);

	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	UFUNCTION(BlueprintCallable)
	void SendPingCheck();

private:
	bool isConnect = false;
	const float PacketProcessInterval = 1.0f / 60.0f;

	FTimerHandle PingTimerHandle;
	FTimerHandle RecvTimerHandle;

public:
	UPROPERTY(BlueprintAssignable, Category = "Login")
	FOnConnectGameServer OnConnectGameServer;
};
