// Copyright Off World Live Limited, 2020-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LivestreamingCameraLog, Log, All);

class FLivestreamingCameraModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	FDelegateHandle OnFEngineLoopInitCompleteHandle;
	void OnFEngineLoopInitComplete();

	FDelegateHandle OnPreExitHandle;
	void OnPreExit();
};
