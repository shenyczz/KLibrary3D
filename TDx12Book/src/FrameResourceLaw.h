#pragma once

#include "FrameResource.h"

struct _FrameResourceLaw : public FrameResource
{
	_FrameResourceLaw(ID3D12Device* device) : FrameResource(device) {}

	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 World = DXUtils::Identity4x4;
	};

	struct PassConstants
	{
		XMFLOAT4X4 View = DXUtils::Identity4x4;
		XMFLOAT4X4 InvView = DXUtils::Identity4x4;
		XMFLOAT4X4 Proj = DXUtils::Identity4x4;
		XMFLOAT4X4 InvProj = DXUtils::Identity4x4;
		XMFLOAT4X4 ViewProj = DXUtils::Identity4x4;
		XMFLOAT4X4 InvViewProj = DXUtils::Identity4x4;
		XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float cbPerObjectPad1 = 0.0f;
		XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
		XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float TotalTime = 0.0f;
		float DeltaTime = 0.0f;
	};

	struct RenderItem
	{
		RenderItem(int dirtys)
			: Dirtys(dirtys)
			, ObjCBIndex(-1) {}

		// 脏标记数
		int Dirtys;

		// 常量缓冲区索引
		UINT ObjCBIndex;

		// 物体世界坐标
		XMFLOAT4X4 World = DXUtils::Identity4x4;

		// 物体网格
		MeshGeometry* Geo = nullptr;

		// 图元拓扑
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// DrawIndexedInstanced parameters.
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};

	//@EndOf(_FrameResourceLaw)
};




struct FrameResourceLaw : public _FrameResourceLaw
{
public:
	FrameResourceLaw(ID3D12Device* device, UINT passCount, UINT objectCount, UINT waveVertCount)
		: _FrameResourceLaw(device)
	{
		PassCB = std::make_unique<TUploadBuffer<PassConstants>>(device, passCount, true);
		ObjectCB = std::make_unique<TUploadBuffer<ObjectConstants>>(device, objectCount, true);

		WavesVB = std::make_unique<TUploadBuffer<Vertex>>(device, waveVertCount, false);
	}

    // We cannot update a cbuffer until the GPU is done processing the commands
    // that reference it.  So each frame needs their own cbuffers.
    std::unique_ptr<TUploadBuffer<PassConstants>> PassCB = nullptr;
    std::unique_ptr<TUploadBuffer<ObjectConstants>> ObjectCB = nullptr;

    // We cannot update a dynamic vertex buffer until the GPU is done processing
    // the commands that reference it.  So each frame needs their own.
    std::unique_ptr<TUploadBuffer<Vertex>> WavesVB = nullptr;

	//@EndOf(FrameResourceLaw)
};