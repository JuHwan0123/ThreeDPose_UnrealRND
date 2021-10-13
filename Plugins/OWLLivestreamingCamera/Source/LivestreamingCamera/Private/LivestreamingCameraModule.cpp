// Copyright Off World Live Limited, 2020-2021. All rights reserved.

#include "LivestreamingCameraModule.h"
#include "USpout/Public/SpoutInterface.h"
#include "Misc/CoreDelegates.h"

#define LOCTEXT_NAMESPACE "FLivestreamingCameraModule"

DEFINE_LOG_CATEGORY(LivestreamingCameraLog);

void FLivestreamingCameraModule::StartupModule()
{
	OnFEngineLoopInitCompleteHandle = FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FLivestreamingCameraModule::OnFEngineLoopInitComplete);
	OnPreExitHandle = FCoreDelegates::OnPreExit.AddRaw(this, &FLivestreamingCameraModule::OnPreExit);
}

void FLivestreamingCameraModule::ShutdownModule()
{
	FCoreDelegates::OnFEngineLoopInitComplete.Remove(OnFEngineLoopInitCompleteHandle);
	FCoreDelegates::OnPreExit.Remove(OnPreExitHandle);
}

void FLivestreamingCameraModule::OnFEngineLoopInitComplete()
{
	USpoutInterface::OpenSpout();
}

void FLivestreamingCameraModule::OnPreExit()
{
	USpoutInterface::CloseSpout();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLivestreamingCameraModule, LivestreamingCamera)
