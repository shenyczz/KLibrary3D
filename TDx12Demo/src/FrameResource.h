#pragma once

enum class PsoStyle : uint16
{
	Default = 0,
	Solid,
	WireFrame,
};



struct FrameResourceBase
{

protected:
	FrameResourceBase()
		: FenceValue(0), CmdListAllocator(nullptr) {}

	FrameResourceBase(ID3D12Device* device)
	{
		if (device)
		{
			ThrowIfFailed(device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(CmdListAllocator.GetAddressOf())));
		}
	}

public:
	// 每帧需要自己的围栏值
	UINT64 FenceValue;

	// 每帧需要自己的命令分配器
	ComPtr<ID3D12CommandAllocator> CmdListAllocator;

};








#pragma region --Box


struct FrameResourceBox : public FrameResourceBase
{
	// 对象常量
	struct ObjectConstants
	{
		XMFLOAT4X4 WorldViewProj = DXUtil::Identity4x4;
	};

	// 过程常量
	struct PassConstants
	{

	};


	FrameResourceBox(ID3D12Device* device)
		:FrameResourceBase(device)
	{
	}







};



#pragma endregion

#pragma region --Shapes

struct FrameResourceShapes : public FrameResourceBase
{
	// 对象常量
	struct ObjectConstants
	{
		XMFLOAT4X4 World = DXUtil::Identity4x4;
	};

	// 过程常量
	struct PassConstants
	{

	};

	struct RenderItem
	{
		RenderItem()
			: World(DXUtil::Identity4x4)
			, NumFramesDirty(0)
			, ObjCBIndex(-1)
			, Geo(nullptr)
			, PrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {}

		// World matrix of the shape that describes the object's local space
		// relative to the world space, which defines the position, orientation,
		// and scale of the object in the world.
		XMFLOAT4X4 World;

		// Dirty flag indicating the object data has changed and we need to update the constant buffer.
		// Because we have an object cbuffer for each FrameResource, we have to apply the
		// update to each FrameResource.  Thus, when we modify obect data we should set 
		// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
		//int NumFramesDirty = gNumFrameResources;
		int NumFramesDirty;

		// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
		UINT ObjCBIndex;

		MeshGeometry* Geo;

		// Primitive topology.
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType;

		// DrawIndexedInstanced parameters.
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		INT BaseVertexLocation = 0;
		//SubmeshGeometry Submesm;

	};


	FrameResourceShapes(ID3D12Device* device)
		:FrameResourceBase(device)
	{
	}







};



#pragma endregion


