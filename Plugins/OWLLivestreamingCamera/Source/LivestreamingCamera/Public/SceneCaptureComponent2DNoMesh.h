// Copyright Off World Live Limited, 2020-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "SceneCaptureComponent2DNoMesh.generated.h"


UCLASS(hidecategories=(Collision, Object, Physics, SceneComponent), ClassGroup=Rendering, editinlinenew, meta=(BlueprintSpawnableComponent))
class LIVESTREAMINGCAMERA_API USceneCaptureComponent2DNoMesh : public USceneCaptureComponent2D {

	GENERATED_BODY()

public:
	USceneCaptureComponent2DNoMesh(const FObjectInitializer& ObjectInitializer);

	bool MeshHidden;

	virtual void OnRegister() override;
};
