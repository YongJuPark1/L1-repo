// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/LyraPlayerController.h"
#include "L1PlayerController.generated.h"

UCLASS()
class LYRAGAME_API AL1PlayerController : public ALyraPlayerController
{
    GENERATED_BODY()
public:
    AL1PlayerController();

protected:
    virtual void BeginPlay() override;
    

};