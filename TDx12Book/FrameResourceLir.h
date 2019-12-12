#pragma once

#include "FrameResource.h"

struct _FrameResourceLir : public FrameResource
{
	_FrameResourceLir(ID3D12Device* device) : FrameResource(device) {}

#pragma pack(1)

	// ������
	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 World = DXUtils::Identity4x4;
	};

	// ���ʳ���
	struct MaterialConstants
	{
		XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;
		XMFLOAT4X4 MatTransform = DXUtils::Identity4x4;
	};

	// ���̳���
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

		// ������
		XMFLOAT4 AmbientLight = { 0.26f, 0.26f, 0.26f, 1.0f };

		// Indices [0, NUM_DIR_LIGHTS) are directional lights;
		// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
		// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
		// are spot lights for a maximum of MaxLights per object.
		Light Lights[MaxLights];
	};

	// ��Ⱦ��Ŀ
	struct RenderItem
	{
		RenderItem(int dirtys) : Dirtys(dirtys) {}

		// ������(֡��Դ����)
		int Dirtys;

		// ������������
		XMFLOAT4X4 World = DXUtils::Identity4x4;

		// ��������������
		UINT ObjCBIndex = -1;

		// ����
		Material* Mat = nullptr;

		// ����
		MeshGeometry* Geo = nullptr;

		// ͼԪ����
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		// ���Ʋ���
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

	// CB - ����������
	std::unique_ptr<TUploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<TUploadBuffer<ObjectConstants>> ObjectCB = nullptr;
	std::unique_ptr<TUploadBuffer<MaterialConstants>> MaterialCB = nullptr;

	//@EndOf(FrameResourceLir)
};

