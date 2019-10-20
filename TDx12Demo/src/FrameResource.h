#pragma once

enum class PsoStyle : uint16
{
	Default = 0,
	Solid,
	WireFrame,
};

// 渲染项
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
	int NumFramesDirty;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	// 用于计算此渲染项对应的GPU常量缓冲区
	UINT ObjCBIndex;

	// World matrix of the shape that describes the object's local space
	// relative to the world space, which defines the position, orientation,
	// and scale of the object in the world.
	XMFLOAT4X4 World;

	// 网格几何体
	MeshGeometry* Geo;

	// 图元拓扑.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType;

	// DrawIndexedInstanced parameters.
	UINT IndexCount;
	UINT StartIndexLocation;
	int BaseVertexLocation;

	//@EndOf(RenderItem)
};


// Stores the resources needed for the CPU to build the command lists for a frame.
// 
struct FrameResourceBase
{
protected:
	FrameResourceBase(ID3D12Device* device)
	{
		FenceValue = 0;
		CmdListAllocator = nullptr;
		if (device)
		{
			ThrowIfFailed(device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(CmdListAllocator.GetAddressOf())));
		}
	}

public:
	~FrameResourceBase() {}

public:
	// 每帧需要自己的围栏值
	UINT64 FenceValue;

	// 每帧需要自己的命令分配器
	ComPtr<ID3D12CommandAllocator> CmdListAllocator;

	//@EndOf(FrameResourceBase)
};


struct FrameResource
{
public:
	FrameResource(ID3D12Device* device)
	{
		FenceValue = 0;
		CmdListAllocator = nullptr;
		if (device)
		{
			ThrowIfFailed(device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(CmdListAllocator.GetAddressOf())));
		}
	}

	~FrameResource() {}



private:
	void Init() {}


public:
	// 每帧需要自己的围栏值
	UINT64 FenceValue;

	// 每帧需要自己的命令分配器
	ComPtr<ID3D12CommandAllocator> CmdListAllocator;

	//@EndOf(FrameResource)

};


