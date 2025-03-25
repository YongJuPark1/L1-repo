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

class LYRAGAME_API TCPResponse
{
public:
	TCPResponse();
	~TCPResponse();
};
