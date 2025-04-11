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
    ingameUserResult.Reset();
    
    Super::EndPlay(EndPlayReason);
}

void AL1PlayerState::AddKill()
{
    // 킬 수 증가
    if (!ingameUserResult.IsValid())
        return;

    UE_LOG(LogLyraExperience, Log, TEXT("Player %s before a kill! Kill: %d"), *ingameUserResult->Nick, ingameUserResult->ingamePlayInfo.killCnt);

    ingameUserResult->ingamePlayInfo.killCnt++;

    UE_LOG(LogLyraExperience, Log, TEXT("Player %s got a kill! Kill: %d"), *ingameUserResult->Nick, ingameUserResult->ingamePlayInfo.killCnt);
}

void AL1PlayerState::AddDeath()
{
    if (!ingameUserResult.IsValid())
        return;

    ingameUserResult->ingamePlayInfo.deathCnt++;

    UE_LOG(LogLyraExperience, Log, TEXT("Player %s got a death! Death %d"), *ingameUserResult->Nick, ingameUserResult->ingamePlayInfo.deathCnt);
}

void AL1PlayerState::AddAssist()
{
    if (!ingameUserResult.IsValid())
        return;

    ingameUserResult->ingamePlayInfo.assistCnt++;

    UE_LOG(LogLyraExperience, Log, TEXT("Player %s got a assist! Assist %d"), *ingameUserResult->Nick, ingameUserResult->ingamePlayInfo.assistCnt);
}


