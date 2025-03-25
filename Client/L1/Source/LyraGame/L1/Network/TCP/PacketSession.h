// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"

class LYRAGAME_API SendBuffer : public TSharedFromThis<SendBuffer>
{
public:
	SendBuffer(int32 bufferSize);
	~SendBuffer();

	
	unsigned char* Buffer() { return _buffer.GetData(); }
	int32 WriteSize() { return _writeSize; }
	int32 Capacity() { return static_cast<int32>(_buffer.Num()); }

	void CopyData(void* data, int32 len);
	void Close(uint32 writeSize);
private:
	TArray<unsigned char> _buffer;
	int32 _writeSize = 0;

};


class LYRAGAME_API PacketSession : public TSharedFromThis<PacketSession>
{
public:
	PacketSession(class FSocket* Socket);
	~PacketSession();

	void Run();
	void HandleRecvPackets();
	void SendPacket(TSharedPtr<class SendBuffer> SendBuffer);
	void Disconnect();
	void Decode(const TArray<uint8>& SerializedData);

public:
	class FSocket* Socket;

	TSharedPtr<class RecvWorker> RecvWorkerThread;
	TSharedPtr<class SendWorker> SendWorkerThread;

	TQueue<TArray<uint8>> RecvPacketQueue;
	TQueue<TSharedPtr<SendBuffer>> SendPacketQueue;

public:
	template<typename T>
	static T Unpack(const TArray<uint8>& SerializedData);

private:
	const int32 MaxPacketsPerFrame = 10;
};
