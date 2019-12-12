/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: ������.֣�� (shenyczz@163.com)
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

//#include "MeshData.h"

namespace dx12
{
	using namespace dxCommon;

	struct SubmeshGeometry
	{
		SubmeshGeometry()
			: IndexCount(0)
			, StartIndexLocation(0)
			, BaseVertexLocation(0) {}

		SubmeshGeometry(int _indexCount, int _startIndexLocation, int _baseVertexLocation)
			: IndexCount(_indexCount)
			, StartIndexLocation(_startIndexLocation)
			, BaseVertexLocation(_baseVertexLocation) {}

		SubmeshGeometry(int _indexCount, int _startIndexLocation, int _baseVertexLocation, const BoundingBox& bbox)
			: IndexCount(_indexCount)
			, StartIndexLocation(_startIndexLocation)
			, BaseVertexLocation(_baseVertexLocation)
			, Bounds(bbox) {}

		// ��������
		UINT IndexCount;

		// ������ʼλ��
		UINT StartIndexLocation;

		// ��׼����λ��
		UINT BaseVertexLocation;

		// ��Χ��
		BoundingBox Bounds;

		//@EndOf(SubmeshGeometry)
	};

}//dx12



namespace dx12
{
	// ���񼸺���
	// MeshGeometry ���԰Ѷ��������Ķ������ݺ��������ݷֱ�洢�ڴ�Ķ��㻺����������������
	//
	struct DX12LIBCOMMON_EXPORTS_CLASS MeshGeometry
	{
	public:
		MeshGeometry()
			: MeshGeometry("") {}

		MeshGeometry(_astring name)
			: Name(name)
			, VertexBufferCPU(nullptr), IndexBufferCPU(nullptr)
			, VertexBufferGPU(nullptr), IndexBufferGPU(nullptr)
			, VertexBufferUploader(nullptr), IndexBufferUploader(nullptr)
			, VBSizeInBytes(0), VBStrideInBytes(0)
			, IBSizeInBytes(0), IBVFormat(DXGI_FORMAT_UNKNOWN) {}

		~MeshGeometry() { DisposeUploader(); }

	public:
		void Init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList, MeshData* pMeshData);

		// ���㻺������ͼ
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
		{
			D3D12_VERTEX_BUFFER_VIEW vbv = {};

			try
			{
				vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
				vbv.SizeInBytes = VBSizeInBytes;
				vbv.StrideInBytes = VBStrideInBytes;
			}
			catch (const std::exception&)
			{
			}

			return vbv;
		}

		// ������������ͼ
		D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
		{
			D3D12_INDEX_BUFFER_VIEW ibv = {};

			try
			{
				ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
				ibv.SizeInBytes = IBSizeInBytes;
				ibv.Format = IBVFormat;
			}
			catch (const std::exception&)
			{
			}

			return ibv;
		}

		void DisposeUploader()
		{
			VertexBufferUploader = nullptr;
			IndexBufferUploader = nullptr;
		}



	public:
		// �������������ƣ����Ը��ݴ������ҵ���
		_astring Name;

		// ϵͳ�ڴ��еĸ�����
		// ���ڶ��㣨�����������Ƿ��͸�ʽ�������ʽ���û����壩��
		// ����ʹ��Blob��������ʾ���û�ʹ��ʱ�轫��ת��Ϊ�ʵ�����

		UINT VBSizeInBytes;								// ���㻺�����ֽڴ�С
		UINT VBStrideInBytes;							// �������ֽ���
		ComPtr<ID3DBlob> VertexBufferCPU;				// VBCPU
		ComPtr<ID3D12Resource> VertexBufferGPU;			// VBGPU


		UINT IBSizeInBytes;								// �����������ֽڴ�С
		DXGI_FORMAT IBVFormat;							// ������ʽ
		ComPtr<ID3DBlob> IndexBufferCPU;				// IBCPU
		ComPtr<ID3D12Resource> IndexBufferGPU;			// IBGPU


		ComPtr<ID3D12Resource> VertexBufferUploader;	// VBUpload
		ComPtr<ID3D12Resource> IndexBufferUploader;		// IBUpload

		// �Ѷ������Ķ��㣨���������ݺϲ���ʹ������
		// �����������������������ԣ����ܵ������Ƴ����е�������

		std::unordered_map<_astring, SubmeshGeometry> DrawArgs;



		//@EndOf(MeshGeometry)
	};

}//dx12


