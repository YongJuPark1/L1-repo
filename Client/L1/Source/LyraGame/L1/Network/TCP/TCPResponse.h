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
	int64 idx;

	UPROPERTY(BlueprintReadWrite)
	int32 itemType;

	UPROPERTY(BlueprintReadWrite)
	int32 itemId;

	UPROPERTY(BlueprintReadWrite)
	bool isEquip;

	MSGPACK_DEFINE(idx, itemType, itemId, isEquip);

};

USTRUCT(BlueprintType)
struct FMonetary
{
	GENERATED_BODY()
public:


	UPROPERTY(BlueprintReadWrite)
	int32 montary;

	UPROPERTY(BlueprintReadWrite)
	int32 amount;

	MSGPACK_DEFINE(montary, amount);

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

	UPROPERTY(BlueprintReadWrite)
	TArray<FMonetary> monetaryList;

	MSGPACK_DEFINE(result.packetName, result.resCode, usn, channelId, nick, monetaryList, charList, weaponList, weaponPartsList);
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

USTRUCT(BlueprintType)
struct FResIngameUserInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	int64 usn;

	UPROPERTY(BlueprintReadWrite)
	int32 matchingType;

	UPROPERTY(BlueprintReadWrite)
	int32 isAI;

	UPROPERTY(BlueprintReadWrite)
	int32 teamId;

	UPROPERTY(BlueprintReadWrite)
	int32 kill;

	UPROPERTY(BlueprintReadWrite)
	int32 death;

	UPROPERTY(BlueprintReadWrite)
	int32 assist;

	UPROPERTY(BlueprintReadWrite)
	int32 win;

	UPROPERTY(BlueprintReadWrite)
	int32 lose;

	

	MSGPACK_DEFINE(usn, matchingType, isAI, teamId, kill, death, assist, win, lose);
};


USTRUCT(BlueprintType)
struct FResIngameUserResult
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FResponse result;

	UPROPERTY(BlueprintReadWrite)
	FResIngameUserInfo ingameUserInfo;

	UPROPERTY(BlueprintReadWrite)
	int32 addRatingScore;

	UPROPERTY(BlueprintReadWrite)
	int32 addGold;

	MSGPACK_DEFINE(result.packetName, result.resCode, ingameUserInfo.usn, ingameUserInfo.matchingType, ingameUserInfo.isAI, ingameUserInfo.teamId, ingameUserInfo.kill, ingameUserInfo.death, ingameUserInfo.assist, ingameUserInfo.win, ingameUserInfo.lose, addRatingScore, addGold);
};



class LYRAGAME_API TCPResponse
{
public:
	TCPResponse();
	~TCPResponse();
};
