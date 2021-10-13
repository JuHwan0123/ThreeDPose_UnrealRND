// Copyright Off World Live Limited, 2020-2021. All rights reserved.

#include "OWLLivestreamingCamera.h"
#include "LivestreamingCameraModule.h"
#include "USpout/Public/SpoutInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/PostProcessComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Interfaces/IPluginManager.h"
#include "ProjectDescriptor.h"

// Sets default values
AOWLLivestreamingCamera::AOWLLivestreamingCamera(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	// create capture component for the 2D output
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DummyRoot;
	CaptureComponent = CreateCaptureComponent(ObjectInitializer);
	CameraName = GetFName().ToString();

	// set up a default camera for when this camera component is possessed
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("PreviewCamera"));
	Camera->SetupAttachment(RootComponent);

	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("OWLLivestreamingCamera");
	if (Plugin.IsValid())
	{
		FPluginDescriptor Desc = Plugin->GetDescriptor();
		PluginVersion = Desc.VersionName;
		EngineVersion = Desc.EngineVersion;
	}
#if WITH_EDITOR
	static ConstructorHelpers::FObjectFinder<UStaticMesh> OWLCameraMesh(TEXT("StaticMesh'/OWLLivestreamingCamera/Meshes/OffWorldCamera.OffWorldCamera'"));
	UStaticMesh* CameraMesh = OWLCameraMesh.Object;
	Camera->SetCameraMesh(CameraMesh);
#endif

	SetFOVAngle(90);
	SetbUseRayTracingIfEnabled(true);
	SetCaptureSource(ESceneCaptureSource::SCS_FinalColorLDR);
	SetTemporalAA(true);
	SetTAAMotionBlur(true);
	SetPostProcessBlendWeight(1.0f);
	SetbCaptureEveryFrame(true);
}

void AOWLLivestreamingCamera::VisitOffWorldLiveWebsite()
{
	FString URL = FString(TEXT("https://offworld.live/livestreaming-camera"));
	FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

void AOWLLivestreamingCamera::SetCameraName(FString NewCameraName)
{
	if (GetWorld() != nullptr && GetWorld()->HasBegunPlay()) USpoutInterface::CloseSender(OldCameraName);
	CameraName = NewCameraName;
	OldCameraName = NewCameraName;
}

FString AOWLLivestreamingCamera::GetCameraName()
{
	return CameraName;
}

void AOWLLivestreamingCamera::SetCameraEnabled(bool NewCameraEnabled)
{
	CameraEnabled = NewCameraEnabled;

	if (!NewCameraEnabled)
	{
		USpoutInterface::CloseSender(CameraName);
		CaptureComponent->Deactivate();
	}
	else CaptureComponent->Activate(false);
}

bool AOWLLivestreamingCamera::GetCameraEnabled()
{
	return CameraEnabled;
}

void AOWLLivestreamingCamera::SetFOVAngle(float NewFOVAngle)
{
	FOVAngle = NewFOVAngle;
	CaptureComponent->FOVAngle = FOVAngle;
	Camera->FieldOfView = FOVAngle;
}

float AOWLLivestreamingCamera::GetFOVAngle()
{
	return FOVAngle;
}

void AOWLLivestreamingCamera::SetCompositeMode(TEnumAsByte<enum ESceneCaptureCompositeMode> NewCompositeMode)
{
	CompositeMode = NewCompositeMode;
	CaptureComponent->CompositeMode = CompositeMode;
}

TEnumAsByte<enum ESceneCaptureCompositeMode> AOWLLivestreamingCamera::GetCompositeMode()
{
	return CompositeMode;
}

void AOWLLivestreamingCamera::SetPostProcessSettings(struct FPostProcessSettings NewPostProcessSettings)
{
	PostProcessSettings = NewPostProcessSettings;
	CaptureComponent->PostProcessSettings = PostProcessSettings;
	Camera->PostProcessSettings = PostProcessSettings;
}

struct FPostProcessSettings AOWLLivestreamingCamera::GetPostProcessSettings()
{
	return PostProcessSettings;
}

void AOWLLivestreamingCamera::SetPostProcessBlendWeight(float NewPostProcessBlendWeight)
{
	PostProcessBlendWeight = NewPostProcessBlendWeight;
	CaptureComponent->PostProcessBlendWeight = PostProcessBlendWeight;
	Camera->PostProcessBlendWeight = PostProcessBlendWeight;
}

float AOWLLivestreamingCamera::GetPostProcessBlendWeight()
{
	return PostProcessBlendWeight;
}

void AOWLLivestreamingCamera::SetbEnableClipPlane(bool NewbEnableClipPlane)
{
	bEnableClipPlane = NewbEnableClipPlane;
	CaptureComponent->bEnableClipPlane = NewbEnableClipPlane;
}

bool AOWLLivestreamingCamera::GetbEnableClipPlane()
{
	return bEnableClipPlane;
}

void AOWLLivestreamingCamera::SetClipPlaneBase(FVector NewClipPlaneBase)
{
	ClipPlaneBase = NewClipPlaneBase;
	CaptureComponent->ClipPlaneBase = ClipPlaneBase;
}

FVector AOWLLivestreamingCamera::GetClipPlaneBase()
{
	return ClipPlaneBase;
}

void AOWLLivestreamingCamera::SetClipPlaneNormal(FVector NewClipPlaneNormal)
{
	ClipPlaneNormal = NewClipPlaneNormal;
	CaptureComponent->ClipPlaneNormal = ClipPlaneNormal;
}

FVector AOWLLivestreamingCamera::GetClipPlaneNormal()
{
	return ClipPlaneNormal;
}

void AOWLLivestreamingCamera::SetbConsiderUnrenderedOpaquePixelAsFullyTranslucent(bool NewbConsiderUnrenderedOpaquePixelAsFullyTranslucent)
{
	bConsiderUnrenderedOpaquePixelAsFullyTranslucent = NewbConsiderUnrenderedOpaquePixelAsFullyTranslucent;
	CaptureComponent->bConsiderUnrenderedOpaquePixelAsFullyTranslucent = bConsiderUnrenderedOpaquePixelAsFullyTranslucent;
}

bool AOWLLivestreamingCamera::GetbConsiderUnrenderedOpaquePixelAsFullyTranslucent()
{
	return bConsiderUnrenderedOpaquePixelAsFullyTranslucent;
}

void AOWLLivestreamingCamera::SetbDisableFlipCopyGLES(bool NewbDisableFlipCopyGLES)
{
	bDisableFlipCopyGLES = NewbDisableFlipCopyGLES;
	CaptureComponent->bDisableFlipCopyGLES = bDisableFlipCopyGLES;
}

bool AOWLLivestreamingCamera::GetbDisableFlipCopyGLES()
{
	return bDisableFlipCopyGLES;
}

void AOWLLivestreamingCamera::SetPrimitiveRenderMode(ESceneCapturePrimitiveRenderMode NewPrimitiveRenderMode)
{
	PrimitiveRenderMode = NewPrimitiveRenderMode;
	CaptureComponent->PrimitiveRenderMode = PrimitiveRenderMode;
}

ESceneCapturePrimitiveRenderMode AOWLLivestreamingCamera::GetPrimitiveRenderMode()
{
	return PrimitiveRenderMode;
}

void AOWLLivestreamingCamera::SetCaptureSource(TEnumAsByte<enum ESceneCaptureSource> NewCaptureSource)
{
	CaptureSource = NewCaptureSource;
	CaptureComponent->CaptureSource = CaptureSource;
}

TEnumAsByte<enum ESceneCaptureSource> AOWLLivestreamingCamera::GetCaptureSource()
{
	return CaptureSource;
}

void AOWLLivestreamingCamera::SetbCaptureEveryFrame(uint8 NewbCaptureEveryFrame)
{
	bCaptureEveryFrame = NewbCaptureEveryFrame;
	CaptureComponent->bCaptureEveryFrame = bCaptureEveryFrame;
}

uint8 AOWLLivestreamingCamera::GetbCaptureEveryFrame()
{
	return bCaptureEveryFrame;
}

void AOWLLivestreamingCamera::SetbCaptureOnMovement(uint8 NewbCaptureOnMovement)
{
	bCaptureOnMovement = NewbCaptureOnMovement;
	CaptureComponent->bCaptureOnMovement = bCaptureOnMovement;
}

uint8 AOWLLivestreamingCamera::GetbCaptureOnMovement()
{
	return bCaptureOnMovement;
}

void AOWLLivestreamingCamera::SetbAlwaysPersistRenderingState(bool NewbAlwaysPersistRenderingState)
{
	bAlwaysPersistRenderingState = NewbAlwaysPersistRenderingState;
	CaptureComponent->bAlwaysPersistRenderingState = bAlwaysPersistRenderingState;
}

bool AOWLLivestreamingCamera::GetbAlwaysPersistRenderingState()
{
	return bAlwaysPersistRenderingState;
}

void AOWLLivestreamingCamera::SetHiddenActors(TArray<AActor*> NewHiddenActors)
{
	HiddenActors = NewHiddenActors;
	CaptureComponent->HiddenActors = HiddenActors;
}

TArray<AActor*> AOWLLivestreamingCamera::GetHiddenActors()
{
	return HiddenActors;
}

void AOWLLivestreamingCamera::SetShowOnlyActors(TArray<AActor*> NewShowOnlyActors)
{
	ShowOnlyActors = NewShowOnlyActors;
	CaptureComponent->ShowOnlyActors = ShowOnlyActors;
}

TArray<AActor*> AOWLLivestreamingCamera::GetShowOnlyActors()
{
	return ShowOnlyActors;
}

void AOWLLivestreamingCamera::SetLODDistanceFactor(float NewLODDistanceFactor)
{
	LODDistanceFactor = NewLODDistanceFactor;
	CaptureComponent->LODDistanceFactor = LODDistanceFactor;
}

float AOWLLivestreamingCamera::GetLODDistanceFactor()
{
	return LODDistanceFactor;
}

void AOWLLivestreamingCamera::SetMaxViewDistanceOverride(float NewMaxViewDistanceOverride)
{
	MaxViewDistanceOverride = NewMaxViewDistanceOverride;
	CaptureComponent->MaxViewDistanceOverride = MaxViewDistanceOverride;
}

float AOWLLivestreamingCamera::GetMaxViewDistanceOverride()
{
	return MaxViewDistanceOverride;
}

void AOWLLivestreamingCamera::SetbUseRayTracingIfEnabled(bool NewbUseRayTracingIfEnabled)
{
	bUseRayTracingIfEnabled = NewbUseRayTracingIfEnabled;
	CaptureComponent->bUseRayTracingIfEnabled = bUseRayTracingIfEnabled;
}

bool AOWLLivestreamingCamera::GetbUseRayTracingIfEnabled()
{
	return bUseRayTracingIfEnabled;
}

USceneCaptureComponent2DNoMesh* AOWLLivestreamingCamera::CreateCaptureComponent(const FObjectInitializer& ObjectInitializer)
{
	USceneCaptureComponent2DNoMesh* captureComponent = ObjectInitializer.CreateDefaultSubobject<USceneCaptureComponent2DNoMesh>(this, FName(*FString(TEXT("CaptureComponent2DCamera"))));
	captureComponent->SetupAttachment(DummyRoot);
	return captureComponent;
}

// Only call if resolution changes
void AOWLLivestreamingCamera::ResizeToMatchStreamResolution(FIntPoint OutputSize)
{
	if (GetWorld() == nullptr || !GetWorld()->HasBegunPlay()) return;
	// no need to recreate RT if we are already in the right resolution
	if (CaptureComponent->TextureTarget->SizeX == OutputSize.X && CaptureComponent->TextureTarget->SizeY == OutputSize.Y) return;
	UE_LOG(LivestreamingCameraLog, Warning, TEXT("Setting output texture to %d x %d"), OutputSize.X, OutputSize.Y)
	CaptureComponent->TextureTarget->ResizeTarget(OutputSize.X, OutputSize.Y);
}

FIntPoint AOWLLivestreamingCamera::GetEffectiveOutputSize()
{
	if (UseCustomStreamResolution)
	{
		if(CustomStreamResolution.X > 0 && CustomStreamResolution.Y > 0) return CustomStreamResolution;
		else 
		{
			UE_LOG(LivestreamingCameraLog, Warning, TEXT("One or more dimentions are set to 0; Defaulting to Non-Custom stream resolution"))
		}
	}

	return GetResolutionFromEnum(StreamResolution);
}

void AOWLLivestreamingCamera::SetUseCustomStreamResolution(bool ShouldUseCustomStreamResolution)
{
	UseCustomStreamResolution = ShouldUseCustomStreamResolution;
	ResizeToMatchStreamResolution(GetEffectiveOutputSize());
}

bool AOWLLivestreamingCamera::GetUseCustomStreamResolution()
{
	return UseCustomStreamResolution;
}

void AOWLLivestreamingCamera::SetStreamResolution(EStreamResolution NewStreamResolution)
{
	StreamResolution = NewStreamResolution;
	ResizeToMatchStreamResolution(GetEffectiveOutputSize());
}

EStreamResolution AOWLLivestreamingCamera::GetStreamResolution()
{
	return StreamResolution;
}

void AOWLLivestreamingCamera::SetCustomStreamResolution(FIntPoint NewCustomStreamResolution)
{
	CustomStreamResolution = NewCustomStreamResolution;
	ResizeToMatchStreamResolution(GetEffectiveOutputSize());
}

FIntPoint AOWLLivestreamingCamera::GetCustomStreamResolution()
{
	return CustomStreamResolution;
}

FIntPoint AOWLLivestreamingCamera::GetResolutionFromEnum(EStreamResolution Res)
{
	switch (Res) 
	{
		case EStreamResolution::RS_240p:
			return FIntPoint(426, 240);
		case EStreamResolution::RS_360p:
			return FIntPoint(640, 360);
		case EStreamResolution::RS_480p:
			return FIntPoint(854, 480);
		case EStreamResolution::RS_720p:
			return FIntPoint(1280, 720);
		case EStreamResolution::RS_1080p:
			return FIntPoint(1920, 1080);
		case EStreamResolution::RS_1440p:
			return FIntPoint(2560, 1440);
		case EStreamResolution::RS_4K:
			return FIntPoint(3840, 2160);
		default:
			return FIntPoint(1920, 1080);
		}
}

void AOWLLivestreamingCamera::SetTemporalAA(bool EnableTemporalAA)
{
	TemporalAAEnabled = EnableTemporalAA;
	CaptureComponent->ShowFlags.SetTemporalAA(TemporalAAEnabled);
}

bool AOWLLivestreamingCamera::GetTemportalAAEnabled()
{
	return TemporalAAEnabled;
}

void AOWLLivestreamingCamera::SetTAAMotionBlur(bool EnableTAAMotionBlur)
{
	TAAMotionBlurEnabled = EnableTAAMotionBlur;
	CaptureComponent->ShowFlags.SetMotionBlur(TAAMotionBlurEnabled);
}

bool AOWLLivestreamingCamera::GetTAAMotionBlurEnabled()
{
	return TAAMotionBlurEnabled;
}

void AOWLLivestreamingCamera::PostInitProperties()
{
	Super::PostInitProperties();
}

void AOWLLivestreamingCamera::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	OldCameraName = CameraName;
	SetAllCameraSettingsInternal();
}

void AOWLLivestreamingCamera::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);
	SetCameraName(GetFName().ToString());
}

#if WITH_EDITOR
void AOWLLivestreamingCamera::PostEditChangeProperty(FPropertyChangedEvent& Prop)
{
	Super::PostEditChangeProperty(Prop);

	FName PropertyName = (Prop.Property != NULL) ? Prop.Property->GetFName() : NAME_None;

	ResizeToMatchStreamResolution(GetEffectiveOutputSize());
	SetPostProcessSettings(PostProcessSettings);

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, FOVAngle))
	{
		SetFOVAngle(FOVAngle);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, CompositeMode))
	{
		SetCompositeMode(CompositeMode);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, PostProcessBlendWeight))
	{
		SetPostProcessBlendWeight(PostProcessBlendWeight);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, bEnableClipPlane))
	{
		SetbEnableClipPlane(bEnableClipPlane);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, ClipPlaneBase))
	{
		SetClipPlaneBase(ClipPlaneBase);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, ClipPlaneNormal))
	{
		SetClipPlaneNormal(ClipPlaneNormal);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, bConsiderUnrenderedOpaquePixelAsFullyTranslucent))
	{
		SetbConsiderUnrenderedOpaquePixelAsFullyTranslucent(bConsiderUnrenderedOpaquePixelAsFullyTranslucent);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, bDisableFlipCopyGLES))
	{
		SetbDisableFlipCopyGLES(bDisableFlipCopyGLES);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, PrimitiveRenderMode))
	{
		SetPrimitiveRenderMode(PrimitiveRenderMode);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, CaptureSource))
	{
		SetCaptureSource(CaptureSource);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, bCaptureEveryFrame))
	{
		SetbCaptureEveryFrame(bCaptureEveryFrame);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, bCaptureOnMovement))
	{
		SetbCaptureOnMovement(bCaptureOnMovement);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, bAlwaysPersistRenderingState))
	{
		SetbAlwaysPersistRenderingState(bAlwaysPersistRenderingState);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, HiddenActors))
	{
		SetHiddenActors(HiddenActors);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, ShowOnlyActors))
	{
		SetShowOnlyActors(ShowOnlyActors);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, LODDistanceFactor))
	{
		SetLODDistanceFactor(LODDistanceFactor);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, MaxViewDistanceOverride))
	{
		SetMaxViewDistanceOverride(MaxViewDistanceOverride);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, bUseRayTracingIfEnabled))
	{
		SetbUseRayTracingIfEnabled(bUseRayTracingIfEnabled);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, TemporalAAEnabled))
	{
		SetTemporalAA(TemporalAAEnabled);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, TAAMotionBlurEnabled))
	{
		SetTAAMotionBlur(TAAMotionBlurEnabled);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, CameraName))
	{
		SetCameraName(CameraName);
		return;
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLLivestreamingCamera, CameraEnabled))
	{
		SetCameraEnabled(CameraEnabled);
		return;
	}
}
#endif

// Called when the game starts or when spawned
void AOWLLivestreamingCamera::BeginPlay()
{
	Super::BeginPlay();
	FIntPoint OutputSize = GetEffectiveOutputSize();

	CaptureComponent->TextureTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), OutputSize.X, OutputSize.Y, RenderTargetFormat);
	CaptureComponent->TextureTarget->TargetGamma = 2.2f;
	ResizeToMatchStreamResolution(OutputSize);
	SetAllCameraSettingsInternal();
}

void AOWLLivestreamingCamera::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	USpoutInterface::CloseSender(CameraName);
}

// Called every frame
void AOWLLivestreamingCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RenderFrame();
}

void AOWLLivestreamingCamera::RenderFrame()
{
	if(CameraEnabled) USpoutInterface::Sender(CameraName, CaptureComponent->TextureTarget);
}

void AOWLLivestreamingCamera::SetAllCameraSettingsInternal()
{
	SetCameraEnabled(CameraEnabled);
	SetFOVAngle(FOVAngle);
	SetCompositeMode(CompositeMode);
	SetPostProcessSettings(PostProcessSettings);
	SetPostProcessBlendWeight(PostProcessBlendWeight);
	SetbEnableClipPlane(bEnableClipPlane);
	SetClipPlaneBase(ClipPlaneBase);
	SetClipPlaneNormal(ClipPlaneNormal);
	SetbConsiderUnrenderedOpaquePixelAsFullyTranslucent(bConsiderUnrenderedOpaquePixelAsFullyTranslucent);
	SetbDisableFlipCopyGLES(bDisableFlipCopyGLES);
	SetPrimitiveRenderMode(PrimitiveRenderMode);
	SetCaptureSource(CaptureSource);
	SetbCaptureEveryFrame(bCaptureEveryFrame);
	SetbCaptureOnMovement(bCaptureOnMovement);
	SetbAlwaysPersistRenderingState(bAlwaysPersistRenderingState);
	SetHiddenActors(HiddenActors);
	SetShowOnlyActors(ShowOnlyActors);
	SetLODDistanceFactor(LODDistanceFactor);
	SetMaxViewDistanceOverride(MaxViewDistanceOverride);
	SetbUseRayTracingIfEnabled(bUseRayTracingIfEnabled);
	SetTAAMotionBlur(TAAMotionBlurEnabled);
	SetTemporalAA(TemporalAAEnabled);
}
