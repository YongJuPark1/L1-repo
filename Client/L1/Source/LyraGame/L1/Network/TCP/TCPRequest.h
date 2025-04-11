// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "variant"
#include "vector"
#include "L1/Network/TCP/NetworkWorker.h"
#include "msgpack/msgpack.hpp"
#include "msgpack/adaptor/MsgpackCustomAdaptor.h"


class LYRAGAME_API TCPRequest
{
public:
	std::string GetPacketName()
	{
		return packetName;
	}

	virtual TArray<uint8> Serialize() const
	{
		msgpack::sbuffer Buffer;
		msgpack::packer<msgpack::sbuffer> Packer(&Buffer);

		auto Fields = GetSerializableFields();
		Packer.pack_array(Fields.size());

		for (const auto& Field : Fields)
		{
			Packer.pack(Field);
		}

		TArray<uint8> SerializedData;
		SerializedData.Append((uint8*)Buffer.data(), Buffer.size());
		return SerializedData;
	}

public:
	TCPRequest();
	virtual ~TCPRequest();

protected:
	using SerializableField = std::variant<int8, int16, int32, int64, std::string, float, double, bool>;

	std::string packetName;

	virtual std::vector<SerializableField> GetSerializableFields() const
	{
		return { packetName };
	}
};

class ReqLogin : public TCPRequest
{
public:
	ReqLogin() { packetName = "ReqLogin"; };
	int32 platformType;
	std::string token;

protected:
	std::vector<SerializableField> GetSerializableFields() const override
	{
		auto Fields = TCPRequest::GetSerializableFields();
		Fields.push_back(platformType);
		Fields.push_back(token);
		return Fields;
	}

};

class ReqPing : public TCPRequest
{
public:
	ReqPing() { packetName = "ReqPing"; }
};

class ReqMatching : public TCPRequest
{
public:
	ReqMatching() { packetName = "ReqMatching"; };

	int32 matchingType;

protected:
	std::vector<SerializableField> GetSerializableFields() const override
	{
		auto Fields = TCPRequest::GetSerializableFields();
		Fields.push_back(matchingType);
		return Fields;
	}
};

class ReqMatchingCancel : public TCPRequest
{
public:
	ReqMatchingCancel() { packetName = "ReqMatchingCancel"; }
};

class ReqMatchingUserCount : public TCPRequest
{
public:
	ReqMatchingUserCount() { packetName = "ReqMatchingUserCount"; }
};

class ReqMatchingDone : public TCPRequest
{
public:
	ReqMatchingDone() { packetName = "ReqMatchingDone"; }
};

class ReqSaveIngameResult : public TCPRequest
{
public:
	ReqSaveIngameResult() { packetName = "ReqSaveIngameResult"; }

	std::string ip;
	int32 port;
	int32 idx;
	int32 matchingType;
	std::string ingameResult;

protected:
	std::vector<SerializableField> GetSerializableFields() const override
	{
		auto Fields = TCPRequest::GetSerializableFields();
		Fields.push_back(ip);
		Fields.push_back(port);
		Fields.push_back(idx);
		Fields.push_back(matchingType);
		Fields.push_back(ingameResult);
		return Fields;
	}
};

class ReqDedicateShutdown : public TCPRequest
{
public:
	ReqDedicateShutdown() { packetName = "ReqDedicateShutdown"; }

	int32 dediIdx;


protected:
	std::vector<SerializableField> GetSerializableFields() const override
	{
		auto Fields = TCPRequest::GetSerializableFields();
		Fields.push_back(dediIdx);
		return Fields;
	}
};
