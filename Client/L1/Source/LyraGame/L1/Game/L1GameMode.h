#pragma once

#include "CoreMinimal.h"
#include "GameModes/LyraGameMode.h"
#include "Data/IngameUserInfo.h"
#include "TimerManager.h"
#include "L1GameMode.generated.h"

/**
 * L1 게임의 게임모드 클래스
 */
UCLASS()
class LYRAGAME_API AL1GameMode : public ALyraGameMode
{
    GENERATED_BODY()

    AL1GameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    ~AL1GameMode();

protected:
    virtual void BeginPlay() override;
    virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;    
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
public:
    void CheckClientConnections();

    UFUNCTION(BlueprintCallable)
    void EndMatch();

    void ShutdownStart();
    void ShutdownEnd();
private:
    int64 GetPlayerUsnInOptions(const FString& Options);
    FTimerHandle ShdownTimerHandle;
    FTimerDelegate ShutdownDelegate;

private:

    int32 Timeout;
    FString strUserList;
    double LastActiveTime;
    FTimerHandle TimeoutCheckTimer;
    int32 NumBots;
    int32 Port;
    int32 MatchingType;
    FString IP;
    int32 Pid;
    int32 Idx;

    int32 GetCurrentBotCount();
public:
    // Array of user info
    //UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InGameUserInfo")

    TSharedPtr<TMap<int64, FInGameUserInfo>> ingameUserInfoMap;
    

};