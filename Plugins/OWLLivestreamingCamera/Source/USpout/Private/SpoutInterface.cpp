// Copyright Off World Live Limited, 2020-2021. All rights reserved.

#include "SpoutInterface.h"
#include "SpoutModule.h"
#include "Spout.h"

// DirectX Interface
ID3D11Device* Device11 = nullptr;
ID3D11DeviceContext* DeviceContext11 = nullptr;
ID3D11On12Device* Device11on12 = nullptr;
// Spout Lib Interface
spoutSenderNames* senderNames = nullptr;
spoutDirectX* sdx = nullptr;
// Our Active Senders
UPROPERTY()
TArray<FSpoutResource> ActiveSpoutResources;
bool Initialised = false;
bool RecieverFormatWarningIssued = false;
bool RecieverNoNameWarningIssued = false;

// Local helper functions invisible to the BP user 

void initSpout()
{
	senderNames = new spoutSenderNames;
	sdx = new spoutDirectX;
}

bool CreateD3D11On12Device()
{
	// Grab native d3d12 device that is used by ue4
	ID3D12Device* Device12 = static_cast<ID3D12Device*>(GDynamicRHI->RHIGetNativeDevice());
	UINT DeviceFlags11 = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	HRESULT res = S_OK;

	// Create a d3d11 device and context using the native d3d12 device
	// note: we're not passing an existing d3d12 command queue but perhaps we should?
	// seems to work fine without it, but we might want to revisit when we know the definitive approach
	res = D3D11On12CreateDevice(
		Device12,
		DeviceFlags11,
		nullptr,
		0,
		nullptr,
		0,
		0,
		&Device11,
		&DeviceContext11,
		nullptr
	);

	if (FAILED(res))
	{
		UE_LOG(SpoutLog, Error, TEXT("DX12: D3D11On12CreateDevice FAIL"));
		return false;
	}
	// Grab interface to the d3d11on12 device from the newly created d3d11 device
	res = Device11->QueryInterface(__uuidof(ID3D11On12Device), (void**)&Device11on12);

	if (FAILED(res))
	{
		UE_LOG(SpoutLog, Error, TEXT("Init11on12: failed to query 11on12 device"));
		return false;
	}

	return true;
}

bool GetDevice()
{
	if (!GDynamicRHI)
	{
		UE_LOG(SpoutLog, Error, TEXT("No existing RHI :-( "));
		return false;
	}

	FString RHIName = GDynamicRHI->GetName();

	if (RHIName == TEXT("D3D11"))
	{
		Device11 = (ID3D11Device*)GDynamicRHI->RHIGetNativeDevice();
		Device11->GetImmediateContext(&DeviceContext11);
		UE_LOG(SpoutLog, Display, TEXT("Graphics Device D3D11"));
		return true;
	}

	else if (RHIName == TEXT("D3D12"))
	{
		if (!CreateD3D11On12Device())
		{
			UE_LOG(SpoutLog, Error, TEXT("We are on dx12 but cannot create D3D11On12"));
			return false;
		}
		UE_LOG(SpoutLog, Display, TEXT("Graphics Device D3D11On12"));
		return true;
	}
	UE_LOG(SpoutLog, Error, TEXT("Spout requires D3D11 or D3D12"));
	return false;
}

FSpoutResource CreateSenderResource(FString spoutName, uint32 Width, uint32 Height, DXGI_FORMAT Format)
{
	HANDLE sharedSendingHandle = NULL;
	ID3D11Texture2D* sendingTexture = nullptr;

	if (!sdx->CreateSharedDX11Texture(Device11, Width, Height, Format, &sendingTexture, sharedSendingHandle))
	{
		UE_LOG(SpoutLog, Error, TEXT("SharedDX11Texture creation failed"));
		return FSpoutResource();
	}

	FSpoutResource NewResource;
	NewResource.Name = spoutName;
	NewResource.Width = Width;
	NewResource.Height = Height;
	NewResource.Format = Format;
	NewResource.SharedSenderTexture = sendingTexture;
	NewResource.Handle = sharedSendingHandle;
	NewResource.SpoutType = ESpoutType::ST_Sender;

	return NewResource;
}

bool CreateRegisterSender(FString spoutName, uint32 Width, uint32 Height, DXGI_FORMAT Format)
{
	FSpoutResource SenderStruct = CreateSenderResource(spoutName, Width, Height, Format);

	if (!senderNames->CreateSender(TCHAR_TO_ANSI(*spoutName), Width, Height, SenderStruct.Handle, Format))
	{
		UE_LOG(SpoutLog, Error, TEXT("Failed while creating sender DX11 with sender name : %s"), *spoutName);
		return false;
	}

	// remove old sender register
	auto Predicate = [&](const FSpoutResource InItem) {return InItem.Name == spoutName; };
	ActiveSpoutResources.RemoveAll(Predicate);

	// register new sender
	ActiveSpoutResources.Add(SenderStruct);
	UE_LOG(SpoutLog, Display, TEXT("Created sender DX11 with sender name %s, Width: %i, Height: %i, Format: %i"), *spoutName, Width, Height, int(Format));
	return true;
}

FSpoutResource CreateReceiverResource(FString spoutName, UTextureRenderTarget2D* ReceiverRT)
{
	unsigned int Width;
	unsigned int Height;
	HANDLE sharedHandle;
	unsigned long Format;

	senderNames->GetSenderInfo(TCHAR_TO_ANSI(*spoutName), Width, Height, sharedHandle, Format);

	FSpoutResource NewResource;
	NewResource.Name = spoutName;
	NewResource.Width = Width;
	NewResource.Height = Height;
	NewResource.Format = (DXGI_FORMAT)Format;
	NewResource.SharedSenderTexture = nullptr;
	NewResource.Handle = sharedHandle;
	NewResource.SpoutType = ESpoutType::ST_Receiver;
	NewResource.ReceiverRT = ReceiverRT;
	HRESULT hr = S_OK;
	hr = Device11->OpenSharedResource(NewResource.Handle, __uuidof(ID3D11Resource), (void**)(&NewResource.SharedSenderTexture));

	if (FAILED(hr))
	{
		UE_LOG(SpoutLog, Error, TEXT("Create Receiver: Failed while trying to open shared dx11 resource"));
		return FSpoutResource();
	}

	return NewResource;
}

bool CreateRegisterReceiver(FString spoutName, UTextureRenderTarget2D* ReceiverRT)
{
	FSpoutResource ReceiverStruct = CreateReceiverResource(spoutName, ReceiverRT);

	if (ReceiverStruct.SharedSenderTexture == nullptr) return false;

	// remove old receiver register if exists
	auto Predicate = [&](const FSpoutResource InItem) {return InItem.Name == spoutName; };
	ActiveSpoutResources.RemoveAll(Predicate);

	// register new sender
	ActiveSpoutResources.Add(ReceiverStruct);
	UE_LOG(SpoutLog, Display, TEXT("Created receiver with receiver name %s"), *spoutName);
	return true;
}

bool UpdateRegisteredSender(FString spoutName, uint32 Width, uint32 Height, DXGI_FORMAT Format)
{
	FSpoutResource SenderStruct = CreateSenderResource(spoutName, Width, Height, Format);

	bool Updated = false;
	for (int32 Index = 0; Index != ActiveSpoutResources.Num(); ++Index)
	{
		if (ActiveSpoutResources[Index].Name == spoutName) {
			ActiveSpoutResources[Index].SharedSenderTexture->Release();
			ActiveSpoutResources.RemoveAt(Index, 1, false);
			ActiveSpoutResources.EmplaceAt(Index, SenderStruct);
			Updated = true;
			UE_LOG(SpoutLog, Display, TEXT("Succesfully Updated Sender %s : Width: %i, Height: %i, Format: %i"), *spoutName, Width, Height, int(Format));
			break;
		}
	}

	if (!Updated) UE_LOG(SpoutLog, Warning, TEXT("Update Failed: No Senders Named %s"), *spoutName);
	return Updated;
}

bool WrapDX12Resource(
	const FTexture2DRHIRef& Src,
	ID3D11Resource** Wrapped11Resource)
{
	HRESULT hr = S_OK;
	// Grab reference to the d3d12 native implementation of the texture
	ID3D12Resource* SrcDX12Resource = (ID3D12Resource*)Src->GetTexture2D()->GetNativeResource();
	D3D11_RESOURCE_FLAGS rf11 = {};

	// Create a wrapped resource - or the way to access our d3d12 resource from the d3d11 device
	//note: D3D12_RESOURCE_STATE variables are: (1) the state of the d3d12 resource when we acquire it
	// (when the d3d12) pipeline is finished with it and we are ready to use it in d3d11 and (2) when
	// we are done using it in d3d11 (we release it back to d3d12) these are the states our resource 
	// will be transitioned into
	hr = Device11on12->CreateWrappedResource(
		SrcDX12Resource, &rf11,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT, __uuidof(ID3D11Resource),
		(void**)Wrapped11Resource);

	if (FAILED(hr))
	{
		UE_LOG(SpoutLog, Error, TEXT("create_d3d12_tex: failed to create "));
		return false;
	}

	Device11on12->ReleaseWrappedResources(Wrapped11Resource, 1);
	return true;
}

FSpoutResource* GetRegistredSpout(FString spoutName)
{
	auto Predicate = [&](const FSpoutResource InItem) { return InItem.Name == spoutName; };
	if (ActiveSpoutResources.ContainsByPredicate(Predicate)) return ActiveSpoutResources.FindByPredicate(Predicate);
	return nullptr;
}

void UnregisterSpout(FString spoutName) {
	auto Predicate = [&](const FSpoutResource InItem) { return InItem.Name == spoutName; };
	ActiveSpoutResources.RemoveAll(Predicate);
}

bool IsSpoutResourceRegistered(FString spoutName)
{
	auto Predicate = [&](const FSpoutResource InItem) { return InItem.Name == spoutName; };
	return ActiveSpoutResources.ContainsByPredicate(Predicate);
}

bool DoesSpoutResourceExist(FString spoutName)
{
	if (senderNames == nullptr) return false;
	return senderNames->FindSenderName(TCHAR_TO_ANSI(*spoutName));
}

void ReleaseResources(FString spoutName)
{
	// here really release the sender
	if (senderNames!= nullptr) senderNames->ReleaseSenderName(TCHAR_TO_ANSI(*spoutName));
}

FSpoutResource* PrepareSenderStructForSending(const FTexture2DRHIRef SrcTexture, FString spoutName)
{
	FIntPoint SourceSize = SrcTexture->GetSizeXY();
	DXGI_FORMAT SourceFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	FString RHIName = GDynamicRHI->GetName();
	if (RHIName == TEXT("D3D12"))
	{
		D3D12_RESOURCE_DESC desc;
		ID3D12Resource* NativeTex = (ID3D12Resource*)SrcTexture->GetNativeResource();
		desc = NativeTex->GetDesc();
		SourceFormat = desc.Format;
	}
	else // (RHIName == TEXT("D3D11"))
	{
		D3D11_TEXTURE2D_DESC desc;
		ID3D11Texture2D* NativeTex = (ID3D11Texture2D*)SrcTexture->GetNativeResource();
		NativeTex->GetDesc(&desc);
		SourceFormat = desc.Format;
	}

	if (SourceFormat == DXGI_FORMAT_B8G8R8A8_TYPELESS) SourceFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

	if (!DoesSpoutResourceExist(spoutName))
	{
		CreateRegisterSender(spoutName, SourceSize.X, SourceSize.Y, SourceFormat);
	}
	if (!IsSpoutResourceRegistered(spoutName))
	{
		USpoutInterface::CloseSender(spoutName);
		CreateRegisterSender(spoutName, SourceSize.X, SourceSize.Y, SourceFormat);
	}

	FSpoutResource* Sender = GetRegistredSpout(spoutName);
	if (Sender == nullptr)
	{
		UE_LOG(SpoutLog, Display, TEXT("Couldn't Get Registered Sender of name : %s "), *spoutName);
		return nullptr;
	}

	// Check whether texture size or format has changed
	if (SourceSize.X != Sender->Width
		|| SourceSize.Y != Sender->Height
		|| SourceFormat != Sender->Format)
	{
		UpdateRegisteredSender(spoutName, SourceSize.X, SourceSize.Y, SourceFormat);
	}

	return GetRegistredSpout(spoutName);
}

// Public FUNCTIONS
void USpoutInterface::OpenSpout()
{
	ENQUEUE_RENDER_COMMAND(void)(
		[](FRHICommandListImmediate& RHICmdList) {
			//Spout is already initialised
			if (Initialised) return true;

			initSpout();
			if (!GetDevice())
			{
				UE_LOG(SpoutLog, Warning, TEXT("Couldn't Get Device"));
				return false;
			}
			Initialised = true;
			return true;
		});
}

void USpoutInterface::CloseSender(FString spoutName)
{
	if (IsSpoutResourceRegistered(spoutName)) UnregisterSpout(spoutName);
	if (DoesSpoutResourceExist(spoutName)) ReleaseResources(spoutName);
	UE_LOG(SpoutLog, Display, TEXT("Closed sender %s."), *spoutName);
}

void USpoutInterface::CloseSpout()
{
	// Close any remaining senders
	TArray<FString> RecieverNamesToClose;
	TArray<FString> SenderNamesToClose;

	for (FSpoutResource ResourceToClose : ActiveSpoutResources)
	{
		if (ResourceToClose.SpoutType != ESpoutType::ST_Receiver)
		{
			SenderNamesToClose.Add(ResourceToClose.Name);
		}
		else
		{
			RecieverNamesToClose.Add(ResourceToClose.Name);
		}
	}

	for (FString NameToClose : SenderNamesToClose)
	{
		CloseSender(NameToClose);
	}

	for (FString NameToClose : RecieverNamesToClose)
	{
		CloseReceiver(NameToClose);
	}

	ENQUEUE_RENDER_COMMAND(void)(
		[](FRHICommandListImmediate& RHICmdList) {
			UE_LOG(SpoutLog, Display, TEXT("Closing Spout"));

			FString RHIName = GDynamicRHI->GetName();
			if (RHIName == TEXT("D3D12"))
			{
				if (Device11on12 != nullptr)
				{
					Device11on12->Release();
					Device11on12 = nullptr;
				}
				if (Device11 != nullptr)
				{
					Device11->Release();
					Device11 = nullptr;
				}
				if (DeviceContext11 != nullptr)
				{
					DeviceContext11->Release();
					DeviceContext11 = nullptr;
				}
			}
			if (senderNames != nullptr)
			{
				delete senderNames;
				senderNames = nullptr;
			}
			if (sdx != nullptr)
			{
				delete sdx;
				sdx = nullptr;
			}
			Initialised = false;
		});
}

void USpoutInterface::Sender(FString spoutName, UTextureRenderTarget2D* textureRenderTarget2D)
{

	if (textureRenderTarget2D == nullptr)
	{
		UE_LOG(SpoutLog, Warning, TEXT("No TextureRenderTarget2D Selected!!"));
		return;
	}

	ENQUEUE_RENDER_COMMAND(void)(
		[spoutName, textureRenderTarget2D](FRHICommandListImmediate& RHICmdList) {
			if (!Initialised)
			{
				UE_LOG(SpoutLog, Error, TEXT("You need to open spout first"));
				return;
			}
			FTexture2DRHIRef Src = textureRenderTarget2D->Resource->TextureRHI->GetTexture2D();
			// Prepare sending struct
			FSpoutResource* SenderResource = PrepareSenderStructForSending(Src, spoutName);
			if (SenderResource == nullptr)
			{
				UE_LOG(SpoutLog, Error, TEXT("Couldn't prepare sender struct"));
				return;
			}
			// Copy sending texture into shared texture
			ID3D11Texture2D* targetTex = SenderResource->SharedSenderTexture;

			FString RHIName = GDynamicRHI->GetName();
			if (RHIName == TEXT("D3D12"))
			{
				ID3D11Resource* WrappedDX11SrcResource = nullptr;
				if (!WrapDX12Resource(Src, &WrappedDX11SrcResource))
				{
					UE_LOG(SpoutLog, Error, TEXT("Couldn't wrap dx12 resource"));
					return;
				}
				// Get our Source Resource from d3d12 to be available for use with d3d11
				Device11on12->AcquireWrappedResources(&WrappedDX11SrcResource, 1);
				// copy it using the d3d11 context into a d3d11 shared texture
				DeviceContext11->CopyResource(targetTex, WrappedDX11SrcResource);
				// Release the source Resource so it can be used again with d3d12
				Device11on12->ReleaseWrappedResources(&WrappedDX11SrcResource, 1);
				// submit d3d11 commands to the GPU
				DeviceContext11->Flush();
			}
			else // (RHIName == TEXT("D3D11"))
			{
				ID3D11Texture2D* Source = (ID3D11Texture2D*)Src->GetNativeResource();
				DeviceContext11->CopyResource(targetTex, Source);
				DeviceContext11->Flush();
			}
			senderNames->UpdateSender(TCHAR_TO_ANSI(*spoutName), SenderResource->Width, SenderResource->Height, SenderResource->Handle);
		});

	return;
}

void USpoutInterface::Receiver(FString spoutName, UTextureRenderTarget2D* textureRenderTarget2D, bool Force_RGBA8_SRGB)
{
	if (textureRenderTarget2D == nullptr)
	{
		UE_LOG(SpoutLog, Warning, TEXT("No Texture2D Selected!"));
		return;
	}

	if (textureRenderTarget2D->RenderTargetFormat != RTF_RGBA8_SRGB && Force_RGBA8_SRGB)
	{
		textureRenderTarget2D->RenderTargetFormat = RTF_RGBA8_SRGB;
		textureRenderTarget2D->UpdateResource();
		UE_LOG(SpoutLog, Warning, TEXT("Setting format to RTF_RGBA8_SRGB on Render Target %s  "), *textureRenderTarget2D->GetFName().GetPlainNameString());
	}

	ENQUEUE_RENDER_COMMAND(void)(
		[spoutName, textureRenderTarget2D](FRHICommandListImmediate& RHICmdList) {
			if (!Initialised)
			{
				UE_LOG(SpoutLog, Error, TEXT("You need to open spout first"));
				return;
			}

			bool ResourceExists = DoesSpoutResourceExist(spoutName);
			bool ResourceRegistered = IsSpoutResourceRegistered(spoutName);

			if (!ResourceExists)
			{
				if (!RecieverNoNameWarningIssued)
				{
					UE_LOG(SpoutLog, Warning, TEXT("No sender found with the name %s"), *spoutName);
					RecieverNoNameWarningIssued = true;
				}
				
				if(ResourceRegistered) CloseSender(spoutName);
				return;
			}
			if (!ResourceRegistered)
			{
				if (!CreateRegisterReceiver(spoutName, textureRenderTarget2D)) return;
			}

			FSpoutResource* ReciverResource = GetRegistredSpout(spoutName);

			// Update Receiving Render Target
			ID3D11Texture2D* Source = ReciverResource->SharedSenderTexture;
			textureRenderTarget2D->ResizeTarget(ReciverResource->Width, ReciverResource->Height);
			if (ReciverResource->Format != DXGI_FORMAT_B8G8R8A8_UNORM
				&& !RecieverFormatWarningIssued)
			{
				UE_LOG(SpoutLog, Warning, TEXT("Reciever %s is trying to read unsuppoted texture format."), *spoutName);
				UE_LOG(SpoutLog, Warning, TEXT("Try deselecting  Force_RGBA8_SRGB and changing pixel formarts in Render Target Editor"));
				UE_LOG(SpoutLog, Warning, TEXT("Or change format of your sending texture to DXGI_FORMAT_B8G8R8A8_UNORM"));
				RecieverFormatWarningIssued = true;
			}

			FTexture2DRHIRef Target = textureRenderTarget2D->Resource->TextureRHI->GetTexture2D();

			FString RHIName = GDynamicRHI->GetName();
			if (RHIName == TEXT("D3D12"))
			{
				ID3D11Resource* WrappedDX11SrcResource = nullptr;
				if (!WrapDX12Resource(Target, &WrappedDX11SrcResource))
				{
					UE_LOG(SpoutLog, Error, TEXT("Couldn't wrap dx12 resource"));
					return;
				}
				// Get our Source Resource from d3d12 to be available for use with d3d11
				Device11on12->AcquireWrappedResources(&WrappedDX11SrcResource, 1);
				// copy it using the d3d11 context into a d3d11 shared texture
				DeviceContext11->CopyResource(WrappedDX11SrcResource, Source);
				// Release the source Resource so it can be used again with d3d12
				Device11on12->ReleaseWrappedResources(&WrappedDX11SrcResource, 1);
				// submit d3d11 commands to the GPU
				DeviceContext11->Flush();
			}
			else // (RHIName == TEXT("D3D11"))
			{
				
				ID3D11Texture2D* targetTex = (ID3D11Texture2D*)Target->GetNativeResource();
				DeviceContext11->CopyResource(targetTex, Source);
				DeviceContext11->Flush();
			}

		});

	return;
}

void USpoutInterface::CloseReceiver(FString spoutName)
{
	if (IsSpoutResourceRegistered(spoutName)) 
	{
		if (GetRegistredSpout(spoutName)->ReceiverRT != nullptr) GetRegistredSpout(spoutName)->ReceiverRT->UpdateResource();
		UnregisterSpout(spoutName);
	}
	UE_LOG(SpoutLog, Display, TEXT("Closed Receiver %s."), *spoutName);
	RecieverFormatWarningIssued = false;
	RecieverNoNameWarningIssued = false;
}

