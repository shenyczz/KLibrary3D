/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: 申永辰.郑州 (shenyczz@163.com)
*  WebSite: http://github.com/shenyczz/KLibrary
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
******************************************************************************/
#pragma once

namespace dx12
{
	template<typename T>
	class TUploadBuffer
	{

	public:
		// 常量缓冲区必须256字节对齐
		// elementCount - 元素数量
		// isConstantBuffer - 是否常量缓冲区
		TUploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer)
			: m_IsConstantBuffer(isConstantBuffer)
			, m_pMapData(nullptr)
			, m_ElementSizeInBytes(0)
		{
			// Constant buffer elements need to be multiples of 256 bytes.
			// This is because the hardware can only view constant data 
			// at m*256 byte offsets and of n*256 byte lengths. 

			//struct D3D12_CONSTANT_BUFFER_VIEW_DESC
			//{
			//	UINT64 BufferLocation;	// GPU_VIRTUAL_ADDRESS
			//	UINT SizeInBytes;
			//};

			m_ElementSizeInBytes = isConstantBuffer
				? ((sizeof(T) + 255) & (~255))
				: sizeof(T);

			ThrowIfFailed(device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),					// D3D12_HEAP_PROPERTIES*
				D3D12_HEAP_FLAG_NONE,												// D3D12_HEAP_FLAGS
				&CD3DX12_RESOURCE_DESC::Buffer(m_ElementSizeInBytes*elementCount),	// D3D12_RESOURCE_DESC*
				D3D12_RESOURCE_STATE_GENERIC_READ,									// D3D12_RESOURCE_STATES
				nullptr,															// D3D12_CLEAR_VALUE*
				IID_PPV_ARGS(&m_UploadBuffer)));

			ThrowIfFailed(m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pMapData)));
			// We do not need to unmap until we are done with the resource.
			// However, we must not write to the resource while it is in use by
			// the GPU (so we must use synchronization techniques).
		}

		~TUploadBuffer()
		{
			if (m_UploadBuffer != nullptr)
				m_UploadBuffer->Unmap(0, nullptr);

			m_pMapData = nullptr;
		}


	public:
		// 上传缓冲区
		ID3D12Resource* Resource() const
		{
			return m_UploadBuffer.Get();
		}

		// data - CPU 计算的结构数据
		void CopyData(int elementIndex, const T& data)
		{
			memcpy(m_pMapData + elementIndex*m_ElementSizeInBytes, &data, sizeof(T));
		}

	private:
		ComPtr<ID3D12Resource> m_UploadBuffer;	// 上传缓冲区
		UINT8* m_pMapData;						// 上传缓冲区的映射数据指针

		UINT m_ElementSizeInBytes;	// SizeInBytes
		bool m_IsConstantBuffer;	// 是否常量缓冲区


	private:
		TUploadBuffer() = delete;
		TUploadBuffer(const TUploadBuffer& rhs) = delete;
		TUploadBuffer& operator=(const TUploadBuffer& rhs) = delete;

		//@EndOf(TUploadBuffer)
	};

	ALIBAPI(void) TUploadBuffer_test();

}//DX12Library


