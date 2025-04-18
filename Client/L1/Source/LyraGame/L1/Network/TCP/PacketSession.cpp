// Fill out your copyright notice in the Description page of Project Settings.


#include "L1/Network/TCP/PacketSession.h"
#include "L1/Network/TCP/NetworkWorker.h"
#include "msgpack/msgpack.hpp"

#define REGISTER_PACKET_HANDLER(PacketType) \
    if (Cmd == TEXT("Req") + FString(#PacketType).RightChop(4) || Cmd == TEXT("Res") + FString(#PacketType).RightChop(4)) \
    { \
        using Type = PacketType; \
        ClientPacketHandler::Handle_##PacketType(Session, PacketSession::Unpack<Type>(BodyData)); \
    }

class PacketRegistry
{
public:
	static void HandlePacket(TSharedPtr<PacketSession> Session, const FString& Cmd, const TArray<uint8>& BodyData)
	{	
		REGISTER_PACKET_HANDLER(FResPing);
		REGISTER_PACKET_HANDLER(FResLogin);
		REGISTER_PACKET_HANDLER(FResMatching);
		REGISTER_PACKET_HANDLER(FResMatchingUserCount);
		REGISTER_PACKET_HANDLER(FResMatchingDone);
		REGISTER_PACKET_HANDLER(FResIngameUserResult);
		REGISTER_PACKET_HANDLER(FResSaveIngameResult);
		REGISTER_PACKET_HANDLER(FResDedicateShutdown);
	}
};


PacketSession::PacketSession(FSocket* Socket) : Socket(Socket)
{
}

PacketSession::~PacketSession()
{
	Disconnect();
}


void PacketSession::Run()
{
	RecvWorkerThread = MakeShared<RecvWorker>(Socket, AsShared());
	SendWorkerThread = MakeShared<SendWorker>(Socket, AsShared());
}



void PacketSession::HandleRecvPackets()
{
	// 한 프레임당 처리할 최대 패킷 수 제한
	int32 ProcessedPackets = 0;
	while (!RecvPacketQueue.IsEmpty() &&
		ProcessedPackets < MaxPacketsPerFrame)
	{
		TArray<uint8> Packet;
		if (RecvPacketQueue.Dequeue(Packet))
		{
			Decode(Packet);
			ProcessedPackets++;
		}
	}
}


void PacketSession::SendPacket(TSharedPtr<class SendBuffer> SendBuffer)
{
	SendPacketQueue.Enqueue(SendBuffer);
}

void PacketSession::Disconnect()
{
	if (RecvWorkerThread)
	{
		RecvWorkerThread->Destroy();
		RecvWorkerThread.Reset();
	}

	if (SendWorkerThread)
	{
		SendWorkerThread->Destroy();
		SendWorkerThread.Reset();
	}
}


void PacketSession::Decode(const TArray<uint8>& SerializedData)
{
	if (SerializedData.Num() < 6) // 최소 6바이트(4바이트 + 2바이트) 필요
	{
		UE_LOG(LogTemp, Error, TEXT("SerializedData is too small to contain required fields!"));
		return;
	}

	const uint8* DataPtr = SerializedData.GetData();


	int32 PacketSize = 0;
	FMemory::Memcpy(&PacketSize, DataPtr, sizeof(PacketSize));
	DataPtr += sizeof(PacketSize);

	int16 CmdLength = 0;
	FMemory::Memcpy(&CmdLength, DataPtr, sizeof(CmdLength));
	DataPtr += sizeof(CmdLength);

	if (CmdLength < 0 || SerializedData.Num() < 6 + CmdLength)
	{
		UE_LOG(LogTemp, Error, TEXT("SerializedData does not contain enough data for the specified cmd length!"));
		return;
	}

	FString cmd = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(DataPtr)));
	if (cmd != "ReqPing")
	{	
		FString DebugMessage = FString::Printf(TEXT("PACKET CMD : %s"), *cmd);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, DebugMessage);
		UE_LOG(LogTemp, Log, TEXT("Send Complete PacketSize : %s"), *cmd);
	}

	DataPtr += CmdLength;

	int16 NonceLength = 0;
	FMemory::Memcpy(&NonceLength, DataPtr, sizeof(NonceLength));
	DataPtr += sizeof(NonceLength);

	int32 BodyLength = 0;
	FMemory::Memcpy(&BodyLength, DataPtr, sizeof(BodyLength));
	DataPtr += sizeof(BodyLength);

	TArray<uint8> BodyData;

	if (BodyLength < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("BodyLength < 0"));
		return;
	}

	BodyData.Append(DataPtr, BodyLength);
	DataPtr += BodyLength;

	int32 TotalExpectedSize = 4 + 2 + CmdLength + 2 + 4 + BodyLength; // 헤더 전체 + BodyLength
	if (SerializedData.Num() < TotalExpectedSize)
	{
		UE_LOG(LogTemp, Error, TEXT("SerializedData size is %d but expected %d! Not enough bytes to unpack."),
			SerializedData.Num(), TotalExpectedSize);
		return;
	}

	
	PacketRegistry::HandlePacket(AsShared(), cmd, BodyData);
}

template<typename T>
inline T PacketSession::Unpack(const TArray<uint8>& SerializedData)
{
	try
	{
		T Packet;
		msgpack::unpacked Unpacked;
		msgpack::unpack(Unpacked, reinterpret_cast<const char*>(SerializedData.GetData()), SerializedData.Num());
		Unpacked.get().convert(Packet);
		return Packet;
	}
	catch (const std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("Unpacking error: %s"), ANSI_TO_TCHAR(e.what()));
		return T();
	}
}


SendBuffer::SendBuffer(int32 bufferSize)
{
	_buffer.SetNum(bufferSize);
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::CopyData(void* data, int32 len)
{
	::memcpy(_buffer.GetData(), data, len);
	_writeSize = len;
}

void SendBuffer::Close(uint32 writeSize)
{
	_writeSize = writeSize;
}