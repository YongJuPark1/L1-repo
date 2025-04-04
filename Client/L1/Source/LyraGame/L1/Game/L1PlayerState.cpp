// L1PlayerState.cpp
#include "L1PlayerState.h"
#include "L1GameInstance.h"
#include "L1/Game/L1GameMode.h"
#include "LyraLogChannels.h"

AL1PlayerState::AL1PlayerState(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    
}

void AL1PlayerState::BeginPlay()
{
    Super::BeginPlay();
}

void AL1PlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (ingameUserInfo != nullptr)
    {
        delete ingameUserInfo;
        ingameUserInfo = nullptr;
        
    }
    Super::EndPlay(EndPlayReason);
}

void AL1PlayerState::AddKill()
{
    // 킬 수 증가
    if (ingameUserInfo == nullptr)
        return;

    UE_LOG(LogLyraExperience, Log, TEXT("Player %s before a kill! Kill: %d"), *ingameUserInfo->Nick, ingameUserInfo->ingamePlayInfo.killCnt);

    ingameUserInfo->ingamePlayInfo.killCnt++;

    // 로그 출력
    UE_LOG(LogLyraExperience, Log, TEXT("Player %s got a kill! Kill: %d"), *ingameUserInfo->Nick, ingameUserInfo->ingamePlayInfo.killCnt);
}

void AL1PlayerState::AddDeath()
{
    if (ingameUserInfo == nullptr)
        return;

    ingameUserInfo->ingamePlayInfo.deathCnt++;

    // 로그 출력
    UE_LOG(LogLyraExperience, Log, TEXT("Player %s got a death! Death %d"), *ingameUserInfo->Nick, ingameUserInfo->ingamePlayInfo.deathCnt);
}

void AL1PlayerState::AddAssist()
{
    // 킬 수 증가
    ingameUserInfo->ingamePlayInfo.assistCnt++;

    // 로그 출력
    UE_LOG(LogLyraExperience, Log, TEXT("Player %s got a assist! Assist %d"), *ingameUserInfo->Nick, ingameUserInfo->ingamePlayInfo.assistCnt);
}


