#pragma once

#include "CoreMinimal.h"
#include "ModularGameMode.h"
#include "LyraGame/GameModes/LyraGameMode.h"
#include "L1GameMode.generated.h"

/**
 * L1 게임의 게임모드 클래스
 */
UCLASS()
class L1_API AL1GameMode : public ALyraGameMode
{
    GENERATED_BODY()

public:
    AL1GameMode();

protected:
    virtual void BeginPlay() override;
}; 