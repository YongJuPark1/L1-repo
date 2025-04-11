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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginResponse, FResLogin, res);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchingResponse, FResMatching, res);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchingCancelResponse, FResMatchingCancel, res);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchingDoneResponse, FResMatchingDone, res);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchingUserCountResponse, FResMatchingUserCount, res);


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
	FString IpAddress = TEXT("192.168.100.11");

	int16 Port = 10501;
	TSharedPtr<class PacketSession> GameServerSession;

protected:
	virtual void Init() override;

public:
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

	UFUNCTION(BlueprintCallable)
	void SendMatching(int32 _matchingType);

	UFUNCTION(BlueprintCallable)
	void SendMatchingCancel();
	
	UFUNCTION(BlueprintCallable)
	void SendMatchingDone();

	UFUNCTION()
	void SendMatchingResult(FString ip, int32 port, int32 pid, int32 matchingType, FString ingameResult);

	UFUNCTION()
	void SendShutdownDedicateServer(int32 dediIdx);

public:
	UFUNCTION(BlueprintCallable)
	void SetUSN(const FText& USN);

	UFUNCTION(BlueprintCallable)
	void SetNickname(const FString& Nick);

	UFUNCTION(BlueprintCallable)
	void LoginProcess();

	void SetPlayerUSN(int64 _playerUSN) { playerUSN = _playerUSN; }

public:
	UFUNCTION(BlueprintCallable)
	int64 GetPlayerUSN() { return playerUSN; }

public:
	FString strUSN;
	FString strSessionId;
	FString strNick;

private:
	int64 playerUSN;

private:
	bool isConnect = false;
	const float PacketProcessInterval = 1.0f / 60.0f;

	FTimerHandle PingTimerHandle;
	FTimerHandle RecvTimerHandle;

public:
	UPROPERTY(BlueprintAssignable, Category = "Login")
	FOnConnectGameServer OnConnectGameServer;

	UPROPERTY(BlueprintAssignable, Category = "Login")
	FOnLoginResponse OnLoginResponse;

	UPROPERTY(BlueprintAssignable, Category = "Matching")
	FOnMatchingResponse OnMatchingResponse;

	UPROPERTY(BlueprintAssignable, Category = "Matching")
	FOnMatchingCancelResponse OnMatchingCancelResponse;

	UPROPERTY(BlueprintAssignable, Category = "Matching")
	FOnMatchingDoneResponse OnMatchingDoneResponse;

	UPROPERTY(BlueprintAssignable, Category = "Matching")
	FOnMatchingUserCountResponse OnMatchingUserCountResponse;

	
};
