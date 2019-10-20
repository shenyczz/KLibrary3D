#pragma once

#include "FrameResource.h"

struct __FrameResourceBox : public FrameResourceBase
{
	__FrameResourceBox(ID3D12Device* device)
		: FrameResourceBase(device) {}

	// 对象常量
	struct ObjectConstants
	{
		XMFLOAT4X4 WorldViewProj = DXUtil::Identity4x4;
	};


	//@EndOf(__FrameResourceBox)
};



struct FrameResourceBox : public __FrameResourceBox
{
	FrameResourceBox(ID3D12Device* device, UINT objectCount)
		: __FrameResourceBox(device)
		, m_ObjectCount(objectCount)
		, ObjectCBs(nullptr)
	{
		ObjectCBs = std::make_unique<TUploadBuffer<ObjectConstants>>(device, objectCount, true);
	}


	unique_ptr<TUploadBuffer<FrameResourceBox::ObjectConstants>> ObjectCBs;




private:
	int m_ObjectCount;

	//@EndOf(FrameResourceBox)
};

