#pragma once

#include "FrameResource.h"

struct __FrameResourceShapes : public FrameResourceBase
{
	__FrameResourceShapes(ID3D12Device* device)
		: FrameResourceBase(device) {}

		// 对象常量
	struct ObjectConstants
	{
		XMFLOAT4X4 World = DXUtil::Identity4x4;
	};

	// 过程对象
	struct PassConstants
	{
		XMFLOAT4X4 View = DXUtil::Identity4x4;
		XMFLOAT4X4 Proj = DXUtil::Identity4x4;
		XMFLOAT4X4 ViewProj = DXUtil::Identity4x4;
		

		/*
		DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();

		DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float cbPerObjectPad1 = 0.0f;
		DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
		DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float TotalTime = 0.0f;
		float DeltaTime = 0.0f;
		*/
	};

	struct RenderItem
	{
		RenderItem(int numFramesDirty)
			: NumFramesDirty(numFramesDirty)
			, ObjCBIndex(-1)
			, World(DXUtil::Identity4x4)
			, Geo(nullptr)
			, PrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			, IndexCount(0)
			, StartIndexLocation(0)
			, BaseVertexLocation(0) {}

		// Dirty flag indicating the object data has changed and we need to update the constant buffer.
		// Because we have an object cbuffer for each FrameResource, we have to apply the
		// update to each FrameResource.  Thus, when we modify obect data we should set 
		// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
		int NumFramesDirty;	// 

		// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
		UINT ObjCBIndex;

		// World matrix of the shape that describes the object's local space
		// relative to the world space, which defines the position, orientation,
		// and scale of the object in the world.
		XMFLOAT4X4 World;

		// Geo
		MeshGeometry* Geo;

		// Primitive topology.
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType;

		// DrawIndexedInstanced parameters.
		UINT IndexCount;
		UINT StartIndexLocation;
		int BaseVertexLocation;
	};
};


struct FrameResourceShapes : public __FrameResourceShapes
{
	FrameResourceShapes(ID3D12Device* device, UINT passCount, UINT objectCount)
		: __FrameResourceShapes(device)
		, m_PassCount(passCount)
		, m_ObjectCount(objectCount)
		, PassCBs(nullptr)
		, ObjectCBs(nullptr)
	{
		PassCBs = std::make_unique<TUploadBuffer<PassConstants>>(device, passCount, true);
		ObjectCBs = std::make_unique<TUploadBuffer<ObjectConstants>>(device, objectCount, true);
	}


	std::unique_ptr<TUploadBuffer<PassConstants>> PassCBs;
	std::unique_ptr<TUploadBuffer<ObjectConstants>> ObjectCBs;


	int PassCount() { return m_PassCount; }
	int ObjectCount() { return m_ObjectCount; }

private:
	int m_PassCount;
	int m_ObjectCount;

	//@EndOf(FrameResourceBox)
};

