// Copyright Off World Live Limited, 2020-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SceneCaptureComponent2DNoMesh.h"
#include "Engine/Scene.h"
#include "OWLLivestreamingCamera.generated.h"

// another feature test just for safety

/* Output texture resolution */
UENUM(BlueprintType)
enum class EStreamResolution : uint8 {
	/* 426x240p */
	RS_240p UMETA(DisplayName = "426x240p"),
	/*  640x360p */
	RS_360p UMETA(DisplayName = "640x360p"),
	/* 854x480p */
	RS_480p UMETA(DisplayName = "854x480p"),
	/* 1280x720p */
	RS_720p UMETA(DisplayName = "720p"),
	/* 1920x1080p */
	RS_1080p UMETA(DisplayName = "1080p"),
	/* 2560x1440p */
	RS_1440p UMETA(DisplayName = "1440p"),
	/* 3840x2160p */
	RS_4K UMETA(DisplayName = "4K"),
};

UCLASS()
class LIVESTREAMINGCAMERA_API AOWLLivestreamingCamera : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOWLLivestreamingCamera(const FObjectInitializer& ObjectInitializer);

public:

	UPROPERTY(VisibleAnywhere, Category = "Version and Support", meta = (MultiLine = "true"))
	FString Support = FString("Please visit our website.");

	UPROPERTY(VisibleAnywhere, Category = "Version and Support", meta = (MultiLine = "true"))
	FString NotIncluded = FString("360 Camera, Audience Interactivity Tools.");

	UPROPERTY(VisibleAnywhere, Category = "Version and Support", meta = (MultiLine = "true"))
	FString PluginVersion = FString("");

	UPROPERTY(VisibleAnywhere, Category = "Version and Support", meta = (MultiLine = "true"))
	FString EngineVersion = FString("");

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Version and Support")
	void VisitOffWorldLiveWebsite();

	/* Name by which the camera can be identified in OBS */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Off World Live Livestreaming Camera Settings", meta = (DisplayPriority = "2"))
	FString CameraName;

	UFUNCTION(BlueprintCallable, Category = "Off World Live Livestreaming Camera Settings")
	void SetCameraName(FString NewCameraName);

	UFUNCTION(BlueprintCallable, Category = "Off World Live Livestreaming Camera Settings")
	FString GetCameraName();

	/* Name by which the camera can be identified in OBS */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Off World Live Livestreaming Camera Settings", meta = (DisplayPriority = "2"))
	bool CameraEnabled = true;

	UFUNCTION(BlueprintCallable, Category = "Off World Live Livestreaming Camera Settings")
	void SetCameraEnabled(bool NewCameraEnabled);

	UFUNCTION(BlueprintCallable, Category = "Off World Live Livestreaming Camera Settings")
	bool GetCameraEnabled();

	UFUNCTION(BlueprintCallable, Category = "Off World Live Livestreaming Camera Settings")
	FIntPoint GetEffectiveOutputSize();

	/* Texture resolution for camera render output */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Off World Live Livestreaming Camera Settings", meta = (DisplayPriority = "2"))
	bool UseCustomStreamResolution = false;

	UFUNCTION(BlueprintCallable, Category = "Off World Live Livestreaming Camera Settings")
	void SetUseCustomStreamResolution(bool ShouldUseCustomStreamResolution);

	UFUNCTION(BlueprintCallable, Category = "Off World Live Livestreaming Camera Settings")
	bool GetUseCustomStreamResolution();

	/* Texture resolution for camera render output */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Off World Live Livestreaming Camera Settings", meta = (editcondition = "!UseCustomStreamResolution", DisplayPriority = "2"))
	EStreamResolution StreamResolution = EStreamResolution::RS_1080p;

	UFUNCTION(BlueprintCallable, Category = "Off World Live Livestreaming Camera Settings")
	void SetStreamResolution(EStreamResolution NewStreamResolution);

	UFUNCTION(BlueprintCallable, Category = "Off World Live Livestreaming Camera Settings")
	EStreamResolution GetStreamResolution();

	/* Texture resolution for camera render output */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Off World Live Livestreaming Camera Settings", meta = (editcondition = "UseCustomStreamResolution", ClampMin = "1", DisplayPriority = "2"))
	FIntPoint CustomStreamResolution = FIntPoint(256, 256);

	UFUNCTION(BlueprintCallable, Category = "Off World Live Livestreaming Camera Settings")
	void SetCustomStreamResolution(FIntPoint NewCustomStreamResolution);

	UFUNCTION(BlueprintCallable, Category = "Off World Live Livestreaming Camera Settings")
	FIntPoint GetCustomStreamResolution();

	///////////////// Scene Capture 2D Interface ////////////////////
	/** Camera field of view (in degrees). */
	UPROPERTY(interp, EditAnywhere, Category = SceneCapture, meta = (DisplayName = "Field of View", UIMin = "5.0", UIMax = "170", ClampMin = "0.001", ClampMax = "360.0"))
	float FOVAngle = 90.0f;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetFOVAngle(float NewFOVAngle);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	float GetFOVAngle();

	/** When enabled, the scene capture will composite into the render target instead of overwriting its contents. */
	UPROPERTY(EditAnywhere, Category = SceneCapture)
	TEnumAsByte<enum ESceneCaptureCompositeMode> CompositeMode = ESceneCaptureCompositeMode::SCCM_Overwrite;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetCompositeMode(TEnumAsByte<enum ESceneCaptureCompositeMode> NewCompositeMode);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	TEnumAsByte<enum ESceneCaptureCompositeMode> GetCompositeMode();

	UPROPERTY(interp, Category = PostProcessVolume, meta = (ShowOnlyInnerProperties))
	struct FPostProcessSettings PostProcessSettings;

	UFUNCTION(BlueprintCallable, Category = PostProcessVolume)
	void SetPostProcessSettings(struct FPostProcessSettings NewPostProcessSettings);

	UFUNCTION(BlueprintCallable, Category = PostProcessVolume)
	struct FPostProcessSettings GetPostProcessSettings();

	/** Range (0.0, 1.0) where 0 indicates no effect, 1 indicates full effect. */
	UPROPERTY(interp, Category = PostProcessVolume, meta = (UIMin = "0.0", UIMax = "1.0"))
	float PostProcessBlendWeight = 1.0f;

	UFUNCTION(BlueprintCallable, Category = PostProcessVolume)
	void SetPostProcessBlendWeight(float NewPostProcessBlendWeight);

	UFUNCTION(BlueprintCallable, Category = PostProcessVolume)
	float GetPostProcessBlendWeight();

	/**
	 * Enables a clip plane while rendering the scene capture which is useful for portals.
	 * The global clip plane must be enabled in the renderer project settings for this to work.
	 */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SceneCapture)
	bool bEnableClipPlane;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetbEnableClipPlane(bool NewbEnableClipPlane);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	bool GetbEnableClipPlane();

	/** Base position for the clip plane, can be any position on the plane. */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SceneCapture)
	FVector ClipPlaneBase;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetClipPlaneBase(FVector NewClipPlaneBase);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	FVector GetClipPlaneBase();

	/** Normal for the plane. */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SceneCapture)
	FVector ClipPlaneNormal = FVector(0, 0, 1);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetClipPlaneNormal(FVector NewClipPlaneNormal);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	FVector GetClipPlaneNormal();

	/** Treat unrendered opaque pixels as fully translucent. This is important for effects such as exponential weight fog, so it does not get applied on unrendered opaque pixels. */
	UPROPERTY(EditAnywhere, Category = SceneCapture)
	bool bConsiderUnrenderedOpaquePixelAsFullyTranslucent = false;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetbConsiderUnrenderedOpaquePixelAsFullyTranslucent(bool NewbConsiderUnrenderedOpaquePixelAsFullyTranslucent);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	bool GetbConsiderUnrenderedOpaquePixelAsFullyTranslucent();

	/**
	 * Scene captures render an extra flip pass for LDR color on GLES so that the final output is oriented correctly.
	 * This check disabled the extra flip pass, improving performance, but causes the capture to be flipped vertically.
	 * (Does not affect scene captures on other non-GLES renderers or with non-LDR output)
	 */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = SceneCapture, meta = (DisplayName = "Disable Flip Copy GLES"))
	bool bDisableFlipCopyGLES = false;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetbDisableFlipCopyGLES(bool NewbDisableFlipCopyGLES);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	bool GetbDisableFlipCopyGLES();

	///////////////// Scene Capture 2D Interface ////////////////////

	///////////////// Scene Capture Interface ////////////////////

	/** Controls what primitives get rendered into the scene capture. */
	UPROPERTY(EditAnywhere, Category = SceneCapture)
	ESceneCapturePrimitiveRenderMode PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetPrimitiveRenderMode(ESceneCapturePrimitiveRenderMode NewPrimitiveRenderMode);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	ESceneCapturePrimitiveRenderMode GetPrimitiveRenderMode();

	UPROPERTY(interp, Category = SceneCapture, meta = (DisplayName = "Capture Source"))
	TEnumAsByte<enum ESceneCaptureSource> CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetCaptureSource(TEnumAsByte<enum ESceneCaptureSource> NewCaptureSource);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	TEnumAsByte<enum ESceneCaptureSource> GetCaptureSource();

	/** Whether to update the capture's contents every frame.  If disabled, the component will render once on load and then only when moved. */
	UPROPERTY(EditAnywhere, Category = SceneCapture)
	uint8 bCaptureEveryFrame : 1;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetbCaptureEveryFrame(uint8 NewbCaptureEveryFrame);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	uint8 GetbCaptureEveryFrame();

	/** Whether to update the capture's contents on movement.  Disable if you are going to capture manually from blueprint. */
	UPROPERTY(EditAnywhere, Category = SceneCapture)
	uint8 bCaptureOnMovement : 1;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetbCaptureOnMovement(uint8 NewbCaptureOnMovement);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	uint8 GetbCaptureOnMovement();

	/** Whether to persist the rendering state even if bCaptureEveryFrame==false.  This allows velocities for Motion Blur and Temporal AA to be computed. */
	UPROPERTY(EditAnywhere, Category = SceneCapture, meta = (editcondition = "!bCaptureEveryFrame"))
	bool bAlwaysPersistRenderingState;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetbAlwaysPersistRenderingState(bool NewbAlwaysPersistRenderingState);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	bool GetbAlwaysPersistRenderingState();

	/** The actors to hide in the scene capture. */
	UPROPERTY(EditInstanceOnly, Category = SceneCapture)
	TArray<AActor*> HiddenActors;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetHiddenActors(TArray<AActor*> NewHiddenActors);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	TArray<AActor*> GetHiddenActors();

	/** The only actors to be rendered by this scene capture, if PrimitiveRenderMode is set to UseShowOnlyList.*/
	UPROPERTY(EditInstanceOnly, Category = SceneCapture)
	TArray<AActor*> ShowOnlyActors;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetShowOnlyActors(TArray<AActor*> NewShowOnlyActors);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	TArray<AActor*> GetShowOnlyActors();

	/** Scales the distance used by LOD. Set to values greater than 1 to cause the scene capture to use lower LODs than the main view to speed up the scene capture pass. */
	UPROPERTY(EditAnywhere, Category = PlanarReflection, meta = (UIMin = ".1", UIMax = "10"), AdvancedDisplay)
	float LODDistanceFactor = 1.0f;

	UFUNCTION(BlueprintCallable, Category = PlanarReflection)
	void SetLODDistanceFactor(float NewLODDistanceFactor);

	UFUNCTION(BlueprintCallable, Category = PlanarReflection)
	float GetLODDistanceFactor();

	/** if > 0, sets a maximum render distance override.  Can be used to cull distant objects from a reflection if the reflecting plane is in an enclosed area like a hallway or room */
	UPROPERTY(EditAnywhere, Category = SceneCapture, meta = (UIMin = "100", UIMax = "10000"))
	float MaxViewDistanceOverride = -1;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetMaxViewDistanceOverride(float NewMaxViewDistanceOverride);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	float GetMaxViewDistanceOverride();

	/** Whether to use ray tracing for this capture. Ray Tracing must be enabled in the project. */
	UPROPERTY(EditAnywhere, Category = SceneCapture)
	bool bUseRayTracingIfEnabled = true;

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	void SetbUseRayTracingIfEnabled(bool NewbUseRayTracingIfEnabled);

	UFUNCTION(BlueprintCallable, Category = SceneCapture)
	bool GetbUseRayTracingIfEnabled();

	///////////////// Scene Capture Interface ////////////////////

	/* Toggles Temporal AntiAliasing */
	UPROPERTY(interp, EditAnywhere, Category = PostProcessVolume)
	bool TemporalAAEnabled = true;

	UFUNCTION(BlueprintCallable, Category = PostProcessVolume)
	void SetTemporalAA(bool EnableTemporalAA);
	
	UFUNCTION(BlueprintCallable, Category = PostProcessVolume)
	bool GetTemportalAAEnabled();

	/* Toggles Temporal AntiAliasing Motion BLur */
	UPROPERTY(interp, EditAnywhere, Category = PostProcessVolume)
	bool TAAMotionBlurEnabled = true;

	UFUNCTION(BlueprintCallable, Category = PostProcessVolume)
	void SetTAAMotionBlur(bool EnableTAAMotionBlur);
	
	UFUNCTION(BlueprintCallable, Category = PostProcessVolume)
	bool GetTAAMotionBlurEnabled();

	UPROPERTY()
	USceneCaptureComponent2DNoMesh* CaptureComponent = nullptr;

	/* A camera to show the preview in the viewport */
	UPROPERTY()
	class UCameraComponent* Camera = nullptr;

public:
	virtual void PostRegisterAllComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitProperties() override;
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

private:
	ETextureRenderTargetFormat RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
	USceneComponent* DummyRoot = nullptr;
	USceneCaptureComponent2DNoMesh* CreateCaptureComponent(const FObjectInitializer& ObjectInitializer);
	void ResizeToMatchStreamResolution(FIntPoint OutputSize);
	FIntPoint GetResolutionFromEnum(EStreamResolution Res);
	void RenderFrame();
	void SetAllCameraSettingsInternal();
	FString OldCameraName;
};
