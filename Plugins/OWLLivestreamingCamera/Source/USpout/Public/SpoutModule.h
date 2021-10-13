// Copyright Off World Live Limited, 2020-2021. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(SpoutLog, Log, All);

class FSpoutModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	void* DLLHandle = nullptr;
};