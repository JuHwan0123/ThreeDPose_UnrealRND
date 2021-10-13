// Copyright Off World Live Limited, 2020-2021. All rights reserved.

#pragma once

#include "Engine/TextureRenderTarget2D.h"
#include "Windows/AllowWindowsPlatformTypes.h"

THIRD_PARTY_INCLUDES_START

#define WIN32_LEAN_AND_MEAN

#pragma warning(push)
// macro redefinition in DirectX headers from ThirdParty folder while they are already defined by <winerror.h> included 
// from "Windows/AllowWindowsPlatformTypes.h"
#pragma warning(disable: 4005)
#include <d3d11on12.h>
#pragma warning(pop)

THIRD_PARTY_INCLUDES_END

#include "Windows/HideWindowsPlatformTypes.h"

#include "SpoutInterface.generated.h"

/* Output texture resolution */
UENUM(BlueprintType)
enum class ESpoutType : uint8 {
	ST_Sender,
	ST_Receiver,
	ST_Invalid 
};

struct FSpoutResource
{
	FString Name;
	HANDLE Handle;
	int32 Width;
	int32 Height;
	// Sender Only Stuff
	ID3D11Texture2D* SharedSenderTexture;
	DXGI_FORMAT Format;
	ESpoutType SpoutType;
	UTextureRenderTarget2D* ReceiverRT;

	FSpoutResource()
	{
		Name = FString();
		Handle = NULL;
		Width = 0;
		Height = 0;
		SharedSenderTexture = nullptr;
		Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		SpoutType = ESpoutType::ST_Invalid;
		ReceiverRT = nullptr;
	}
};


UCLASS()
class USPOUT_API USpoutInterface : public UObject
{
	GENERATED_BODY()

public:
	static void OpenSpout();
	static void CloseSpout();

	static void Sender(FString spoutName, UTextureRenderTarget2D* textureRenderTarget2D);
	static void CloseSender(FString spoutName);

	static void Receiver(FString spoutName, UTextureRenderTarget2D* textureRenderTarget2D, bool Force_RGBA8_SRGB);
	static void CloseReceiver(FString spoutName);
};