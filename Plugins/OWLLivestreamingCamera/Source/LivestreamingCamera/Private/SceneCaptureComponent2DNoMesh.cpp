// Copyright Off World Live Limited, 2020-2021. All rights reserved.

#include "SceneCaptureComponent2DNoMesh.h"

USceneCaptureComponent2DNoMesh::USceneCaptureComponent2DNoMesh(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	MeshHidden = true;
	bUseRayTracingIfEnabled = true;
}
// Sets default values for this component's properties
void USceneCaptureComponent2DNoMesh::OnRegister()
{
	Super::OnRegister();
#if WITH_EDITORONLY_DATA
	if (ProxyMeshComponent != nullptr && MeshHidden) {
		ProxyMeshComponent->SetVisibility(false, false);
	}
#endif
	UpdateShowFlags();
}
