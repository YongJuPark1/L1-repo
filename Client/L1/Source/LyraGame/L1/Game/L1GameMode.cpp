#include "L1GameMode.h"
#include "TimerManager.h"
#include "LyraLogChannels.h"
#include "GameModes/LyraGameState.h"
#include "Player/LyraPlayerState.h"
#include "L1/Game/L1PlayerState.h"
#include "Misc/Parse.h" 
#include "Kismet/GameplayStatics.h"
#include "L1/Game/Data/IngameUserInfo.h"
#include "AIController.h"
#include "GameModes/LyraBotCreationComponent.h"
#include "L1GameInstance.h"


AL1GameMode::AL1GameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerStateClass = AL1PlayerState::StaticClass();
}

void AL1GameMode::BeginPlay()
{
    Super::BeginPlay();	
}


void AL1GameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	if (ingameUserInfoMap.Num() > 0)
		ingameUserInfoMap.Empty();
	
	if (FParse::Value(FCommandLine::Get(), TEXT("IP="), IP))
	{
		UE_LOG(LogLyraExperience, Log, TEXT("Property : %s"), *IP);
	}
	else
	{
		UE_LOG(LogLyraExperience, Log, TEXT("Property not found IP"));
	}

	if (FParse::Value(FCommandLine::Get(), TEXT("Port="), Port))
	{	
		UE_LOG(LogLyraExperience, Log, TEXT("Property Port: %d"), Port);
	}
	else
	{
		UE_LOG(LogLyraExperience, Log, TEXT("Property not found Port"));
	}

	if (FParse::Value(FCommandLine::Get(), TEXT("Idx="), Idx))
	{
		UE_LOG(LogLyraExperience, Log, TEXT("Property Idx: %d"), Idx);
	}
	else
	{
		UE_LOG(LogLyraExperience, Log, TEXT("Property not found Idx"));
	}

	if (FParse::Value(FCommandLine::Get(), TEXT("matchingType="), MatchingType))
	{
		UE_LOG(LogLyraExperience, Log, TEXT("Property matchingType: %d"), MatchingType);
	}
	else
	{
		UE_LOG(LogLyraExperience, Log, TEXT("Property not found matchingType"));
	}

	

	if (FParse::Value(FCommandLine::Get(), TEXT("timeout="), Timeout))
	{
		UE_LOG(LogLyraExperience, Log, TEXT("Property TimeOut: %d"), Timeout);
	}
	else
	{
		Timeout = 300;
	}

	if (FParse::Value(FCommandLine::Get(), TEXT("NumBots="), NumBots))
	{
		UE_LOG(LogLyraExperience, Log, TEXT("Property NumBots: %d"), NumBots);
	}
	else
	{
		NumBots = 0;
	}

	this->OptionsString = Options;
	FString AddOptionString = TEXT("?NumBots=");
	
	this->OptionsString.Append(AddOptionString + LexToString(NumBots) + TEXT("?"));

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
				UserInfo.Usn = static_cast<int64>(UserInfoObject->GetIntegerField(TEXT("usn")));
				UserInfo.MatchingType = UserInfoObject->GetIntegerField(TEXT("matching_type"));
				UserInfo.TeamId = UserInfoObject->GetIntegerField(TEXT("team_id"));
				UserInfo.RatingScore = UserInfoObject->GetIntegerField(TEXT("rating_score"));
				UserInfo.IsAI = UserInfoObject->GetIntegerField(TEXT("isAI"));

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


void AL1GameMode::EndMatch()
{
	if (GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server EndMatch"));
		FString DebugMessage = FString::Printf(TEXT("[Server EndMatch Success]"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);
		
		TArray<TSharedPtr<FJsonValue>> UserArray;

		for (const auto& pair : ingameUserInfoMap)
		{
			// 개별 사용자 정보를 저장할 JSON 객체
			TSharedPtr<FJsonObject> UserJsonObject = MakeShared<FJsonObject>();

			UserJsonObject->SetNumberField(TEXT("usn"), pair.Value.Usn);
			UserJsonObject->SetNumberField(TEXT("isAI"), pair.Value.IsAI);
			UserJsonObject->SetNumberField(TEXT("matching_type"), pair.Value.MatchingType);
			UserJsonObject->SetNumberField(TEXT("teamId"), pair.Value.TeamId);			
			UserJsonObject->SetNumberField(TEXT("kill"), pair.Value.ingamePlayInfo.killCnt);			
			UserJsonObject->SetNumberField(TEXT("death"), pair.Value.ingamePlayInfo.deathCnt);			
			UserJsonObject->SetNumberField(TEXT("assist"), pair.Value.ingamePlayInfo.assistCnt);			
			UserJsonObject->SetNumberField(TEXT("win"), pair.Value.ingamePlayInfo.winCnt);			
			UserJsonObject->SetNumberField(TEXT("lose"), pair.Value.ingamePlayInfo.loseCnt);

			// 변환한 JSON 객체를 배열에 추가
			UserArray.Add(MakeShared<FJsonValueObject>(UserJsonObject));
		}

		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		JsonObject->SetArrayField(TEXT("ingameUserInfoList"), UserArray);

		FString ingameResult;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ingameResult);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

		// JSON이 비어있는지 확인
		if (JsonObject->Values.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("JsonObject is empty!"));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Find Start L1GameInstance"));
		UL1GameInstance* L1GameInstance = Cast<UL1GameInstance>(GetGameInstance());
		if (L1GameInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("Found L1GameInstance"));
			UE_LOG(LogTemp, Log, TEXT("EndMatch Property : %s"), *IP);
			UE_LOG(LogTemp, Log, TEXT("EndMatch Property Port: %d"), Port);
			UE_LOG(LogTemp, Log, TEXT("EndMatch Property Idx: %d"), Idx);
			UE_LOG(LogTemp, Log, TEXT("EndMatch Property matchingType: %d"), MatchingType);
			L1GameInstance->SendMatchingResult(IP, Port, Idx, MatchingType,ingameResult);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("L1GameInstance Not Found"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Client EndMatch"));
		FString DebugMessage = FString::Printf(TEXT("[Client EndMatch Success]"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);
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

int32 AL1GameMode::GetCurrentBotCount()
{
	int32 BotCount = 0;

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AController* Controller = It->Get();
		if (Controller && Controller->IsA(AAIController::StaticClass()))  // AI 컨트롤러만 카운트
		{
			BotCount++;
		}
	}

	return BotCount;
}

void AL1GameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	UE_LOG(LogLyraExperience, Log, TEXT("PreLogin called! GameMode Instance: %p"), this);

	int64 playerUSN = GetPlayerUsnInOptions(Options);

	UE_LOG(LogTemp, Log, TEXT("=== IngameUserInfoMap ==="));
	for (const auto& pair : ingameUserInfoMap)
	{
		UE_LOG(LogTemp, Log, TEXT("USN Key: %lld, UserInfo.USN: %lld"), pair.Key, pair.Value.Usn);
	}
	UE_LOG(LogTemp, Log, TEXT("========================="));

	UE_LOG(LogTemp, Log, TEXT("Checking USN: %lld"), playerUSN);

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

	if (FInGameUserInfo* userInfo = ingameUserInfoMap.Find(playerUSN))
	{	
		PlayerState->SetIngameUserInfo(userInfo);
	}

#if WITH_SERVER_CODE 
	if (GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		int32 MaxPlayers = 16;
		int32 CurrentPlayers = GetNumPlayers();
		int32 CurrentBots = GetCurrentBotCount();

		UE_LOG(LogTemp, Log, TEXT("InitNewPlayer called. Players: %d, Bots: %d"), CurrentPlayers, CurrentBots);

		if ((CurrentPlayers + CurrentBots) > MaxPlayers)
		{
			if (ULyraBotCreationComponent* BotCreationComponent = FindComponentByClass<ULyraBotCreationComponent>())
			{
				BotCreationComponent->RemoveOneBot();
				UE_LOG(LogTemp, Log, TEXT("Removed one bot in InitNewPlayer"));
			}
		}
	}
#endif
	

	return FString();
}
