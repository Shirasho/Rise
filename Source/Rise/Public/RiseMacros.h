#pragma once

#include <tuple>
#include "RiseLog.h"

#define NUM_ARGS(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

#define MAKE_NAME(Value) FName(#Value)
#define MAKE_TEXT(Value) #Value

#define DEBUG_LOG(Color, Text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::##Color, ##Text)
#define DEBUG_LOGF(Color, Format, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::##Color, FString::Printf(##Format, ##__VA_ARGS__))

#define RISE_LOG(...) \
	DEBUG_LOG(Cyan, ##__VA_ARGS__); \
	UE_LOG(LogRise, Log, ##__VA_ARGS__) \

#define RISE_LOGF(Format, ...) \
	DEBUG_LOGF(Cyan, Format, ##__VA_ARGS__); \
	UE_LOG(LogRise, Log, Format, ##__VA_ARGS__) \

#define RISE_WARNING(...) \
	DEBUG_LOG(Yellow, ##__VA_ARGS__); \
	UE_LOG(LogRise, Warning, ##__VA_ARGS__) \

#define RISE_WARNINGF(Format, ...) \
	DEBUG_LOGF(Yellow, Format, ##__VA_ARGS__); \
	UE_LOG(LogRise, Warning, Format, ##__VA_ARGS__) \

#define RISE_ERROR(...) \
	DEBUG_LOG(Red, ##__VA_ARGS__); \
	UE_LOG(LogRise, Error, ##__VA_ARGS__) \

#define RISE_ERRORF(Format, ...) \
	DEBUG_LOGF(Red, Format, ##__VA_ARGS__); \
	UE_LOG(LogRise, Error, Format, ##__VA_ARGS__)