#include "L1GameMode.h"
#include "TimerManager.h"

AL1GameMode::AL1GameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void AL1GameMode::BeginPlay()
{
    Super::BeginPlay();
}

void AL1GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UE_LOG(LogTemp, Log, TEXT("LyraGameMode::InitGame called!"));

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);

	if (FParse::Value(FCommandLine::Get(), TEXT("timeout="), Timeout))
	{
		UE_LOG(LogTemp, Log, TEXT("Property TimeOut: %d"), Timeout);
	}
	else
	{
		Timeout = 20;
	}

	UE_LOG(LogTemp, Log, TEXT("TimeOut: %d"), Timeout);

	//���� Ŀ�ؼ��� 300�� ���� ���� ��� ��������Ʈ �ڵ� ���� ��Ų��.
	GetWorld()->GetTimerManager().SetTimer(TimeoutCheckTimer, this, &AL1GameMode::CheckClientConnections, 1.0f, true);
}


void AL1GameMode::CheckClientConnections()
{
	int32 ConnectedPlayers = GetWorld()->GetNumPlayerControllers();

	if (ConnectedPlayers > 0)
	{
		LastActiveTime = GetWorld()->GetTimeSeconds();
	}
	else
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime - LastActiveTime > Timeout)
		{
			// ���� ����
			UE_LOG(LogTemp, Warning, TEXT("No clients connected. Shutting down server..."));
			FGenericPlatformMisc::RequestExit(false);
		}
	}
}