/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: …Í”¿≥Ω.÷£÷› (shenyczz@163.com)
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

namespace DX12
{
	template<typename T>
	class UploadBuffer
	{
	public:
		UploadBuffer(const UploadBuffer& rhs) = delete;
		UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

		UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer)
			: m_IsConstantBuffer(isConstantBuffer)
			, m_MappedData(nullptr)
			, m_ElementByteSize(0)
		{
			// Constant buffer elements need to be multiples of 256 bytes.
			// This is because the hardware can only view constant data 
			// at m*256 byte offsets and of n*256 byte lengths. 
			// typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
			// UINT64 OffsetInBytes; // multiple of 256
			// UINT   SizeInBytes;   // multiple of 256
			// } D3D12_CONSTANT_BUFFER_VIEW_DESC;
			m_ElementByteSize = isConstantBuffer
				? sizeof(T)
				: DXUtil::CalcConstantBufferByteSize(sizeof(T));	// ((sizeof(T)+255) & (~255))

			ThrowIfFailed(device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(m_ElementByteSize*elementCount),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_UploadBuffer)));

			ThrowIfFailed(m_UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedData)));
			// We do not need to unmap until we are done with the resource.
			// However, we must not write to the resource while it is in use by
			// the GPU (so we must use synchronization techniques).
		}

		~UploadBuffer()
		{
			if (m_UploadBuffer != nullptr)
				m_UploadBuffer->Unmap(0, nullptr);

			m_MappedData = nullptr;
		}


	public:
		ID3D12Resource * Resource() const
		{
			return m_UploadBuffer.Get();
		}

		void CopyData(int elementIndex, const T& data)
		{
			memcpy(&m_MappedData[elementIndex*m_ElementByteSize], &data, sizeof(T));
		}

	private:
		ComPtr<ID3D12Resource> m_UploadBuffer;
		BYTE* m_MappedData;

		UINT m_ElementByteSize;
		bool m_IsConstantBuffer;
	};

}//DX12


