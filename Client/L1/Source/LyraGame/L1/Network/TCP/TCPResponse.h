// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "variant"
#include "vector"
#include "msgpack.hpp"
#include "TCPResponse.generated.h"

USTRUCT(BlueprintType)
struct FResponse
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FString packetName;

	UPROPERTY(BlueprintReadWrite)
	int32 resCode;

	MSGPACK_DEFINE(packetName, resCode);
};

USTRUCT(BlueprintType)
struct FResPing
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FResponse result;

	MSGPACK_DEFINE(result.packetName, result.resCode);
};


USTRUCT(BlueprintType)
struct FEquipmentItem
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
	int32 idx;

	UPROPERTY(BlueprintReadWrite)
	int32 itemType;

	UPROPERTY(BlueprintReadWrite)
	int32 itemId;

	UPROPERTY(BlueprintReadWrite)
	bool isEquip;

	MSGPACK_DEFINE(idx, itemType, itemId, isEquip);

};


USTRUCT(BlueprintType)
struct FResLogin
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FResponse result;

	UPROPERTY(BlueprintReadWrite)
	int64 usn;

	UPROPERTY(BlueprintReadWrite)
	int32 channelId;

	UPROPERTY(BlueprintReadWrite)
	FString nick;

	UPROPERTY(BlueprintReadWrite)
	TArray<FEquipmentItem> charList;

	UPROPERTY(BlueprintReadWrite)
	TArray<FEquipmentItem> weaponList;

	UPROPERTY(BlueprintReadWrite)
	TArray<FEquipmentItem> weaponPartsList;

	MSGPACK_DEFINE(result.packetName, result.resCode, usn, channelId, nick, charList, weaponList, weaponPartsList);
};


USTRUCT(BlueprintType)
struct FResMatching
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FResponse result;

	MSGPACK_DEFINE(result.packetName, result.resCode);
};

USTRUCT(BlueprintType)
struct FResMatchingCancel
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FResponse result;

	MSGPACK_DEFINE(result.packetName, result.resCode);
};

USTRUCT(BlueprintType)
struct FResMatchingUserCount
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FResponse result;

	UPROPERTY(BlueprintReadWrite)
	int32 userCount;

	MSGPACK_DEFINE(result.packetName, result.resCode, userCount);
};

USTRUCT(BlueprintType)
struct FResMatchingDone
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FResponse result;

	UPROPERTY(BlueprintReadWrite)
	FString ip;

	UPROPERTY(BlueprintReadWrite)
	int32 port;

	UPROPERTY(BlueprintReadWrite)
	FString key;

	MSGPACK_DEFINE(result.packetName, result.resCode, ip, port, key);
};

class LYRAGAME_API TCPResponse
{
public:
	TCPResponse();
	~TCPResponse();
};
