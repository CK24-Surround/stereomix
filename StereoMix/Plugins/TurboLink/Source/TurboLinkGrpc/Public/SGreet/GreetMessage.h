//Generated by TurboLink CodeGenerator, do not edit!
#pragma once
#include "TurboLinkGrpcMessage.h"
#include "GreetMessage.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName="Greet.HelloRequest"))
struct FGrpcGreetHelloRequest : public FGrpcMessage
{
    GENERATED_BODY()
    DECLARE_JSON_FUNCTIONS()

    UPROPERTY(BlueprintReadWrite, Category = TurboLink)
    FString Name = "";
};

USTRUCT(BlueprintType, meta = (DisplayName="Greet.HelloResponse"))
struct FGrpcGreetHelloResponse : public FGrpcMessage
{
    GENERATED_BODY()
    DECLARE_JSON_FUNCTIONS()

    UPROPERTY(BlueprintReadWrite, Category = TurboLink)
    FString Message = "";
};
