#pragma once


// 帧资源
//
struct FrameResource
{
protected:
    FrameResource(ID3D12Device* device)
		: CmdListAllocator(nullptr), FenceValue(0)
	{
		ThrowIfFailed(device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&CmdListAllocator)));
	}

public:
	// 每帧需要自己的命令分配器
    ComPtr<ID3D12CommandAllocator> CmdListAllocator;

	// 每帧的围栏值
	UINT64 FenceValue;


	//@EndOf(FrameResource)
};