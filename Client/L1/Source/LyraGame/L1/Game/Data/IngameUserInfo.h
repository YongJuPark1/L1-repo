// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IngameUserInfo.generated.h"

USTRUCT(BlueprintType)
struct FIngamePlayInfo
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InGamePlayInfo")
    int32 killCnt = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InGamePlayInfo")
    int32 deathCnt = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InGamePlayInfo")
    int32 assistCnt = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InGamePlayInfo")
    int32 winCnt = 0;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InGamePlayInfo")
    int32 loseCnt = 0;
};

USTRUCT(BlueprintType)
struct FInGameUserInfo
{
    GENERATED_BODY()

public:
    // Nickname of the user
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InGameUserInfo")
    FString Nick;

    // User ID
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InGameUserInfo")
    int32 Usn;

    // Team ID
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InGameUserInfo")
    int32 TeamId;

    // Rating score
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InGameUserInfo")
    int32 RatingScore;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InGameUserInfo")
    FIngamePlayInfo ingamePlayInfo;
};
