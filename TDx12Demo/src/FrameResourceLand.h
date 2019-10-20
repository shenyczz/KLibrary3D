#pragma once

#include "FrameResource.h"

struct __FrameResourceLand : public FrameResourceBase
{
	__FrameResourceLand(ID3D12Device* device)
		: FrameResourceBase(device) {}

	// ������
	struct ObjectConstants
	{
		XMFLOAT4X4 World = DXUtil::Identity4x4;
	};

	// ���̳���
	struct PassConstants
	{
		XMFLOAT4X4 View = DXUtil::Identity4x4;
		XMFLOAT4X4 Proj = DXUtil::Identity4x4;
		XMFLOAT4X4 ViewProj = DXUtil::Identity4x4;
	};

	//@EndOf(__FrameResourceBox)
};



struct FrameResourceLand : public __FrameResourceLand
{
	FrameResourceLand(ID3D12Device* device, UINT objectCount, UINT waveVertCount)
		: __FrameResourceLand(device)
		, m_ObjectCount(objectCount)
		, PassCB(nullptr)
		, ObjectCB(nullptr)
		, WavesVB(nullptr)
	{
		PassCB = std::make_unique<TUploadBuffer<PassConstants>>(device, 1, true);
		ObjectCB = std::make_unique<TUploadBuffer<ObjectConstants>>(device, objectCount, true);
		WavesVB = std::make_unique<TUploadBuffer<Vertex>>(device, waveVertCount, false);
	}

	//FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT waveVertCount);

	// �ϴ�������
	unique_ptr<TUploadBuffer<FrameResourceLand::PassConstants>> PassCB;
	unique_ptr<TUploadBuffer<FrameResourceLand::ObjectConstants>> ObjectCB;


	//��̬����
	//WavesVB = std::make_unique<UploadBuffer<Vertex>>(device, waveVertCount, false);
	// We cannot update a dynamic vertex buffer until the GPU is done processing
	// the commands that reference it.  So each frame needs their own.
	std::unique_ptr<TUploadBuffer<Vertex>> WavesVB = nullptr;


private:
	int m_ObjectCount;

	//@EndOf(FrameResourceBox)
};

