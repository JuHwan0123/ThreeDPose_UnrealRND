// Copyright Off World Live Limited, 2020-2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Tickable.h"
#include "OWLSpoutReceiver.generated.h"


struct FReceiverTickHelper : FTickableGameObject
{
	TWeakObjectPtr<class AOWLSpoutReceiver> Owner;

	FReceiverTickHelper() : Owner(NULL) {}
	virtual void Tick(float DeltaTime);
	virtual bool IsTickable() const { return Owner.IsValid(); }
	virtual bool IsTickableInEditor() const { return true; }
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual TStatId GetStatId() const;
};

UCLASS()
class LIVESTREAMINGCAMERA_API AOWLSpoutReceiver : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOWLSpoutReceiver(const FObjectInitializer& ObjectInitializer);

	FDelegateHandle OnLevelActorDeletedHandle;
	void OnLevelActorDeleted(AActor* DestroyedActor);

public:

	virtual void TickMe(float DeltaTime);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Off World Live Spout Receiver Settings")
	bool ReceiverActive = false;

	UFUNCTION(BlueprintCallable, Category = "Off World Live Spout Receiver Settings")
	void SetReceiverActive(bool ActivateReceiver);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Off World Live Spout Receiver Settings")
	UTextureRenderTarget2D* RenderTarget;

	UFUNCTION(BlueprintCallable, Category = "Off World Live Spout Receiver Settings")
	void SetRenderTarget(UTextureRenderTarget2D* NewRenderTarget);

	UFUNCTION(BlueprintCallable, Category = "Off World Live Spout Receiver Settings")
	UTextureRenderTarget2D* GetRenderTarget();

	/* Name by which the receiver can be identified in OBS */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Off World Live Spout Receiver Settings")
	FString ReceiverName = FString::FString("");

	UFUNCTION(BlueprintCallable, Category = "Off World Live Spout Receiver Settings")
	void SetReceiverName(FString NewReceiverName);

	UFUNCTION(BlueprintCallable, Category = "Off World Live Spout Receiver Settings")
	FString GetReceiverName();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Off World Live Spout Receiver Settings")
	bool Force_RGBA8_SRGB = true;

public:
	virtual void PostRegisterAllComponents() override;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

private:
	USceneComponent* DummyRoot = nullptr;
	FReceiverTickHelper TickHelper;
	void RenderFrame();
	FString OldReceiverName;
};
