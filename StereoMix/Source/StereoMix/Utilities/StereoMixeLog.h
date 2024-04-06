
#pragma once

DECLARE_LOG_CATEGORY_CLASS(LogStereoMixNetwork, Log, All);

#if UE_BUILD_SHIPPING
#define NET_LOG(NetOwner, Verbosity, Format, ...)
#else
#define NET_LOG(NetOwner, Verbosity, Format, ...)\
{\
	const AActor* Macro_NetOwner = NetOwner;\
	FString Macro_NetModeInfo;\
	FString Macro_LocalRoleInfo;\
	FString Macro_RemoteRoleInfo;\
	const FString Macro_FunctionInfo = FString(__FUNCTION__);\
	\
	if (Macro_NetOwner)\
	{\
		const ENetMode Macro_NetMode = Macro_NetOwner->GetNetMode();\
		Macro_NetModeInfo = Macro_NetMode == NM_Client ? FString::Printf(TEXT("Client%d"), static_cast<int>(GPlayInEditorID)) : Macro_NetMode == NM_DedicatedServer ? TEXT("Server") : TEXT("Misc");\
		\
		Macro_LocalRoleInfo = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), Macro_NetOwner->GetLocalRole());\
		Macro_RemoteRoleInfo = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), Macro_NetOwner->GetRemoteRole());\
	}\
	\
	FString Macro_Log;\
	if (Macro_NetOwner)\
	{\
	Macro_Log = FString::Printf(TEXT("[%s] [Local: %s | Remote: %s] [%s]"), *Macro_NetModeInfo, *Macro_LocalRoleInfo, *Macro_RemoteRoleInfo, *Macro_FunctionInfo);\
	}\
	else\
	{\
		Macro_Log = FString::Printf(TEXT("[No Connection] [%s]"), *Macro_FunctionInfo);\
	}\
	\
	const FString Macro_NullText = TEXT("");\
	if (Macro_NullText != Format)\
	{\
		UE_LOG(LogStereoMixNetwork, Verbosity, TEXT(" \n\t%s\n\t%s"), *Macro_Log, *FString::Printf(Format, ##__VA_ARGS__));\
	}\
	else\
	{\
		UE_LOG(LogStereoMixNetwork, Verbosity, TEXT(" \n\t%s"), *Macro_Log);\
	}\
}
#endif