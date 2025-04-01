#include "L1GameMode.h"
#include "TimerManager.h"
#include "LyraLogChannels.h"
#include "GameModes/LyraGameState.h"
#include "Player/LyraPlayerState.h"
#include "L1/Game/L1PlayerState.h"
#include "L1/Game/L1PlayerController.h"
#include "Misc/Parse.h" 
#include "Kismet/GameplayStatics.h"
#include "L1/Game/Data/IngameUserInfo.h"

AL1GameMode::AL1GameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerStateClass = AL1PlayerState::StaticClass();
	PlayerControllerClass = AL1PlayerController::StaticClass();  // 이렇게 설정했다면
}

void AL1GameMode::BeginPlay()
{
    Super::BeginPlay();	
}


void AL1GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UE_LOG(LogLyraExperience, Log, TEXT("LyraGameMode::InitGame called!"));

	if (FParse::Value(FCommandLine::Get(), TEXT("timeout="), Timeout))
	{
		UE_LOG(LogLyraExperience, Log, TEXT("Property TimeOut: %d"), Timeout);
	}
	else
	{
		Timeout = 300;
	}

	UE_LOG(LogLyraExperience, Log, TEXT("TimeOut: %d"), Timeout);

	FString FullCommandLine = FCommandLine::Get();
	FString SearchKey = TEXT("userlist=");
	int32 KeyIndex = FullCommandLine.Find(SearchKey);
	if (KeyIndex != INDEX_NONE)
	{
		strUserList = FullCommandLine.Mid(KeyIndex + SearchKey.Len());

		int32 EndIndex = strUserList.Find(TEXT("}"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		if (EndIndex != INDEX_NONE)
		{
			strUserList = strUserList.Left(EndIndex + 1);  // JSON 부분만 추출
		}

		UE_LOG(LogLyraExperience, Log, TEXT("Extracted JSON: %s"), *strUserList);

		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(strUserList);
		TSharedPtr<FJsonObject> JsonObject;

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
		{
			// JSON에서 "ingame_userinfo_list" 키를 가져오기
			TArray<TSharedPtr<FJsonValue>> UserInfoArray = JsonObject->GetArrayField(TEXT("ingame_userinfo_list"));

			UE_LOG(LogLyraExperience, Log, TEXT("Parsed JSON successfully. User count: %d"), UserInfoArray.Num());

			// 배열을 순회하면서 각 항목을 FInGameUserInfo로 변환
			for (const TSharedPtr<FJsonValue>& Value : UserInfoArray)
			{
				TSharedPtr<FJsonObject> UserInfoObject = Value->AsObject();

				// 사용자 정보 가져오기
				FInGameUserInfo UserInfo;
				UserInfo.Nick = UserInfoObject->GetStringField(TEXT("nick"));
				UserInfo.Usn = UserInfoObject->GetIntegerField(TEXT("usn"));
				UserInfo.TeamId = UserInfoObject->GetIntegerField(TEXT("team_id"));
				UserInfo.RatingScore = UserInfoObject->GetIntegerField(TEXT("rating_score"));

				// 배열에 추가
				ingameUserInfoMap.Add(UserInfo.Usn, UserInfo);
			}

			// 사용자 목록을 로그로 출력
			for (const auto& pair : ingameUserInfoMap)
			{
				const FInGameUserInfo& userInfo = pair.Value;
				UE_LOG(LogLyraExperience, Log, TEXT("User: %s, Usn: %d, TeamId: %d, RatingScore: %d"), *userInfo.Nick, userInfo.Usn, userInfo.TeamId, userInfo.RatingScore);
			}
		}
		else
		{
			UE_LOG(LogLyraExperience, Log, TEXT("FJsonSerializer::Deserialize(JsonReader, JsonObject) Falied"));
		}
	}
	else
	{
		UE_LOG(LogLyraExperience, Log, TEXT("KeyIndex != INDEX_NONE"));
	}

	//유저 커넥션이 300초 동안 없을 경우 데디케이트 자동 종료 시킨다.
	GetWorld()->GetTimerManager().SetTimer(TimeoutCheckTimer, this, &AL1GameMode::CheckClientConnections, 1.0f, true);
}

void AL1GameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{	
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
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
			// 서버 종료
			UE_LOG(LogTemp, Warning, TEXT("No clients connected. Shutting down server..."));
			FGenericPlatformMisc::RequestExit(false);
		}
	}
}

int64 AL1GameMode::GetPlayerUsnInOptions(const FString& Options)
{
	int64 result = 0;

	if (UGameplayStatics::HasOption(Options, TEXT("usn")))
	{
		FString strUsn = UGameplayStatics::ParseOption(Options, TEXT("usn"));
		result = FCString::Strtoi(*strUsn, nullptr, 10);

		UE_LOG(LogTemp, Log, TEXT("USN : %lld"), result);
	}

	return result;
}

void AL1GameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	int64 playerUSN = GetPlayerUsnInOptions(Options);

	if (!ingameUserInfoMap.Contains(playerUSN))
	{
		ErrorMessage = TEXT("접속이 차단되었습니다: 올바른 USN이 필요합니다.");
		UE_LOG(LogTemp, Error, TEXT("PreLogin: Client rejected due to invalid USN (USN=%lld)"), playerUSN);
	}
}


FString AL1GameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	int64 playerUSN = GetPlayerUsnInOptions(Options);

	AL1PlayerState* PlayerState = NewPlayerController->GetPlayerState<AL1PlayerState>();
	AL1PlayerController* PlayerController = Cast<AL1PlayerController>(NewPlayerController);

	if (FInGameUserInfo* userInfo = ingameUserInfoMap.Find(playerUSN))
	{	
		PlayerState->SetIngameUserInfo(userInfo);
	}

	return FString();
}

void AL1GameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	Super::EndPlay(EndPlayReason);

}
