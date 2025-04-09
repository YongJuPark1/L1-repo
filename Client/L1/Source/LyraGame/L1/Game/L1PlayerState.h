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
    // �ʿ��� �߰� �Լ��� ������ ����
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
   

public:
    void SetIngameUserInfo(FInGameUserInfo* _ingameUserInfo) { ingameUserInfo = _ingameUserInfo; }
    FInGameUserInfo* GetIngameUserInfo(){ return ingameUserInfo; }

    void AddKill();
    void AddDeath();
    void AddAssist();
    
private:
    FInGameUserInfo* ingameUserInfo;

};