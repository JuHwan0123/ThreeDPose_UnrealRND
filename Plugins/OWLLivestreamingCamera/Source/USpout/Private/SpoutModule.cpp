// Copyright Off World Live Limited, 2020-2021. All rights reserved.

#include "SpoutModule.h"
#include "Interfaces/IPluginManager.h"
#include "Core.h"

DEFINE_LOG_CATEGORY(SpoutLog);

#define LOCTEXT_NAMESPACE "FSpoutModule"

void FSpoutModule::StartupModule()
{
	FString Path = IPluginManager::Get()
		.FindPlugin("OWLLiveStreamingCamera")->GetBaseDir()
		.Append(FString(TEXT("/Source/ThirdParty/Spout/lib/amd64/Spout.dll")));
	DLLHandle = FPlatformProcess::GetDllHandle(*Path);
}

void FSpoutModule::ShutdownModule()
{
	FPlatformProcess::FreeDllHandle(DLLHandle);
	DLLHandle = nullptr;
}

IMPLEMENT_MODULE(FSpoutModule, USpout)

#undef LOCTEXT_NAMESPACE