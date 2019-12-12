#pragma once

#include "FrameResource.h"

struct _FrameResourceLir : public FrameResource
{
	_FrameResourceLir(ID3D12Device* device) : FrameResource(device) {}

#pragma pack(1)

	// 对象常量
	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 World = DXUtils::Identity4x4;
	};

	// 材质常量
	struct MaterialConstants
	{
		XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;
		XMFLOAT4X4 MatTransform = DXUtils::Identity4x4;
	};

	// 过程常量
	struct PassConstants
	{
		XMFLOAT4X4 View = DXUtils::Identity4x4;
		XMFLOAT4X4 InvView = DXUtils::Identity4x4;
		XMFLOAT4X4 Proj = DXUtils::Identity4x4;
		XMFLOAT4X4 InvProj = DXUtils::Identity4x4;
		XMFLOAT4X4 ViewProj = DXUtils::Identity4x4;
		XMFLOAT4X4 InvViewProj = DXUtils::Identity4x4;

		XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float __cbPerObjectPad1 = 0.0f;

		XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
		XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };

		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float TotalTime = 0.0f;
		float DeltaTime = 0.0f;

		// 环境光
		XMFLOAT4 AmbientLight = { 0.26f, 0.26f, 0.26f, 1.0f };

		// Indices [0, NUM_DIR_LIGHTS) are directional lights;
		// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
		// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
		// are spot lights for a maximum of MaxLights per object.
		Light Lights[MaxLights];
	};

	// 渲染条目
	struct RenderItem
	{
		RenderItem(int dirtys) : Dirtys(dirtys) {}

		// 脏标记数(帧资源数量)
		int Dirtys;

		// 物体世界坐标
		XMFLOAT4X4 World = DXUtils::Identity4x4;

		// 常量缓冲区索引
		UINT ObjCBIndex = -1;

		// 材质
		Material* Mat = nullptr;

		// 网格
		MeshGeometry* Geo = nullptr;

		// 图元拓扑
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// 绘制参数
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};

#pragma pack()


	enum class RenderLayer : int
	{
		Opaque = 0,
		Transparent,
		AlphaTested,

		Count,
	};


	//@EndOf(_FrameResourceLir)
};


struct FrameResourceLir : public _FrameResourceLir
{
	FrameResourceLir(ID3D12Device* device, UINT passCount, UINT objCount, UINT matCount)
		: _FrameResourceLir(device)
	{
		PassCB = std::make_unique<TUploadBuffer<PassConstants>>(device, passCount, true);
		ObjectCB = std::make_unique<TUploadBuffer<ObjectConstants>>(device, objCount, true);
		MaterialCB = std::make_unique<TUploadBuffer<MaterialConstants>>(device, matCount, true);
	}

	// CB - 常量缓冲区
	std::unique_ptr<TUploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<TUploadBuffer<ObjectConstants>> ObjectCB = nullptr;
	std::unique_ptr<TUploadBuffer<MaterialConstants>> MaterialCB = nullptr;

	//@EndOf(FrameResourceLir)
};

