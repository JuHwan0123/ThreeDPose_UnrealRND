// Copyright Off World Live Limited, 2020-2021. All rights reserved.

#include "OWLSpoutReceiver.h"
#include "LivestreamingCameraModule.h"
#include "USpout/Public/SpoutInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Interfaces/IPluginManager.h"
#include "ProjectDescriptor.h"

// The Tick Helper implementation is based on MockAI.h / MockAI.cpp
void FReceiverTickHelper::Tick(float DeltaTime)
{
	if (Owner.IsValid())
	{
		Owner->TickMe(DeltaTime);
	}
}

TStatId FReceiverTickHelper::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FReceiverTickHelper, STATGROUP_Tickables);
}

// Sets default values
AOWLSpoutReceiver::AOWLSpoutReceiver(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DummyRoot;
}

void AOWLSpoutReceiver::OnLevelActorDeleted(AActor* DestroyedActor)
{
#if WITH_EDITOR
	if (DestroyedActor == this)
	{
		UE_LOG(LivestreamingCameraLog, Warning, TEXT("%s is being deleted"), *ReceiverName)
		USpoutInterface::CloseReceiver(ReceiverName);
		check(GEngine);
		GEngine->OnLevelActorDeleted().Remove(OnLevelActorDeletedHandle);
		OnLevelActorDeletedHandle.Reset();
	}
#endif
}

void AOWLSpoutReceiver::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	SetReceiverActive(ReceiverActive);
	OldReceiverName = ReceiverName;
}

void AOWLSpoutReceiver::PostInitProperties()
{
	Super::PostInitProperties();
#if WITH_EDITOR
	if (GEngine)
	{
		OnLevelActorDeletedHandle = GEngine->OnLevelActorDeleted().AddUObject(this, &AOWLSpoutReceiver::OnLevelActorDeleted);
	}
#endif
}

void AOWLSpoutReceiver::BeginDestroy()
{
#if WITH_EDITOR
	if (GEngine)
	{
		GEngine->OnLevelActorDeleted().Remove(OnLevelActorDeletedHandle);
	}
#endif
	Super::BeginDestroy();
}

void AOWLSpoutReceiver::TickMe(float DeltaTime)
{
	RenderFrame();
}

void AOWLSpoutReceiver::SetReceiverActive(bool ActivateReceiver)
{
	ReceiverActive = ActivateReceiver;
	if (ActivateReceiver)
	{
		TickHelper.Owner = this;
		UE_LOG(LivestreamingCameraLog, Warning, TEXT("Receiver %s activated"), *ReceiverName)
	}
	else
	{
		USpoutInterface::CloseReceiver(ReceiverName);
		TickHelper.Owner = NULL;
		UE_LOG(LivestreamingCameraLog, Warning, TEXT("Receiver %s deactivated"), *ReceiverName)
	}
}

void AOWLSpoutReceiver::SetRenderTarget(UTextureRenderTarget2D* NewRenderTarget)
{
	if (NewRenderTarget == nullptr) USpoutInterface::CloseReceiver(ReceiverName);
	RenderTarget = NewRenderTarget;
}

UTextureRenderTarget2D* AOWLSpoutReceiver::GetRenderTarget()
{
	return RenderTarget;
}

void AOWLSpoutReceiver::SetReceiverName(FString NewReceiverName)
{
	if (ReceiverActive) USpoutInterface::CloseReceiver(ReceiverName);
	ReceiverName = NewReceiverName;
	OldReceiverName = NewReceiverName;
}

FString AOWLSpoutReceiver::GetReceiverName()
{
	return ReceiverName;
}

#if WITH_EDITOR
void AOWLSpoutReceiver::PostEditChangeProperty(FPropertyChangedEvent& Prop)
{
	Super::PostEditChangeProperty(Prop);

	FName PropertyName = (Prop.Property != NULL) ? Prop.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLSpoutReceiver, ReceiverActive))
	{
		SetReceiverActive(ReceiverActive);
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLSpoutReceiver, ReceiverName))
	{
		SetReceiverName(ReceiverName);
	}
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOWLSpoutReceiver, RenderTarget))
	{
		SetRenderTarget(RenderTarget);
	}
}
#endif

// Called when the game starts or when spawned
void AOWLSpoutReceiver::BeginPlay()
{
	Super::BeginPlay();
	SetReceiverActive(ReceiverActive);
}

void AOWLSpoutReceiver::RenderFrame()
{
	if (RenderTarget != nullptr && ReceiverName != FString::FString(""))
	{
		USpoutInterface::Receiver(ReceiverName, RenderTarget, Force_RGBA8_SRGB);
	}
}
