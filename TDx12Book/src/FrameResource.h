#pragma once


// ֡��Դ
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
	// ÿ֡��Ҫ�Լ������������
    ComPtr<ID3D12CommandAllocator> CmdListAllocator;

	// ÿ֡��Χ��ֵ
	UINT64 FenceValue;


	//@EndOf(FrameResource)
};