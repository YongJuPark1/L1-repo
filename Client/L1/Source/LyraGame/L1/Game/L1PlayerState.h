#pragma once
#include "CoreMinimal.h"
#include "Player/LyraPlayerState.h"
#include "L1/Game/Data/IngameUserInfo.h"
#include "L1PlayerState.generated.h"

UCLASS()
class LYRAGAME_API AL1PlayerState : public ALyraPlayerState
{
    GENERATED_BODY()

public:
    AL1PlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    // 필요한 추가 함수나 변수들 선언
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
   

public:
    void SetIngameUserInfo(TSharedPtr<FInGameUserInfo> _ingameUserInfo) { ingameUserResult = _ingameUserInfo; }
    TSharedPtr<FInGameUserInfo> GetIngameUserInfo(){ return ingameUserResult; }

    void AddKill();
    void AddDeath();
    void AddAssist();
    
private:
    TSharedPtr<FInGameUserInfo> ingameUserResult;

};