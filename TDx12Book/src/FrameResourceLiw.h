#pragma once

#include "FrameResource.h"


struct _FrameResourceLiw : public FrameResource
{
	_FrameResourceLiw(ID3D12Device* device) : FrameResource(device) {}

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

		// 环境光
		XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

		// Indices [0, NUM_DIR_LIGHTS) are directional lights;
		// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
		// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
		// are spot lights for a maximum of MaxLights per object.
		Light Lights[MaxLights];
	};

	struct MaterialConstants
	{
		XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;
		XMFLOAT4X4 MatTransform = DXUtils::Identity4x4;
	};

	struct RenderItem
	{
		RenderItem(int dirtys) : Dirtys(dirtys) {}

		// 脏标记数
		int Dirtys;

		// 物体世界坐标
		XMFLOAT4X4 World = DXUtils::Identity4x4;

		// 
		XMFLOAT4X4 TexTransform = DXUtils::Identity4x4;

		// 常量缓冲区索引
		UINT ObjCBIndex = -1;

		// 材质
		Material* Mat = nullptr;

		// 网格
		MeshGeometry* Geo = nullptr;

		// 图元拓扑
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// DrawIndexedInstanced parameters.
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};

	//@EndOf(_FrameResourceLiw)
};

struct FrameResourceLiw : public _FrameResourceLiw
{
	FrameResourceLiw(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT waveVertCount)
		: _FrameResourceLiw(device)
	{
		PassCB = std::make_unique<TUploadBuffer<PassConstants>>(device, passCount, true);
		ObjectCB = std::make_unique<TUploadBuffer<ObjectConstants>>(device, objectCount, true);
		MaterialCB = std::make_unique<TUploadBuffer<MaterialConstants>>(device, materialCount, true);

		WavesVB = std::make_unique<TUploadBuffer<Vertex>>(device, waveVertCount, false);
	}

	// CB
	std::unique_ptr<TUploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<TUploadBuffer<MaterialConstants>> MaterialCB = nullptr;
	std::unique_ptr<TUploadBuffer<ObjectConstants>> ObjectCB = nullptr;

	// VB
	std::unique_ptr<TUploadBuffer<Vertex>> WavesVB = nullptr;

	//@EndOf(FrameResourceLiw)
};


/*
struct FrameResourceLiw : public _FrameResourceLiw
{
public:
	FrameResourceLiw(ID3D12Device* device, UINT passCount, UINT objectCount, UINT waveVertCount)
		: _FrameResourceLiw(device)
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
*/

