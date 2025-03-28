#pragma once

#include "CoreMinimal.h"
#include "GameModes/LyraGameMode.h"
#include "L1GameMode.generated.h"

/**
 * L1 게임의 게임모드 클래스
 */
UCLASS()
class LYRAGAME_API AL1GameMode : public ALyraGameMode
{
    GENERATED_BODY()

    AL1GameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    

protected:
    virtual void BeginPlay() override;
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

public:
    void CheckClientConnections();

private:
    int32 Timeout;
    double LastActiveTime;
    FTimerHandle TimeoutCheckTimer;
};