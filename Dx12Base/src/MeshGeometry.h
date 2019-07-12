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

//#include <windows.h>
//#include <wrl.h>
//#include <dxgi1_4.h>
//#include <d3d12.h>
//#include <D3Dcompiler.h>
//#include <DirectXMath.h>
//#include <DirectXPackedVector.h>
//#include <DirectXColors.h>
//#include <DirectXCollision.h>
//#include <string>
//#include <memory>
//#include <algorithm>
//#include <vector>
//#include <array>
//#include <cstdint>
//#include <fstream>
//#include <sstream>
//#include <cassert>
//#include "d3dx12.h"
//#include <unordered_map>

namespace DX12
{
	struct DX12BASE_EXPORTS_CLASS SubmeshGeometry
	{
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		INT BaseVertexLocation = 0;

		DirectX::BoundingBox Bounds;
	};

}//DX12



namespace DX12
{
	struct DX12BASE_EXPORTS_CLASS MeshGeometry
	{
		MeshGeometry();
		~MeshGeometry();

		// �������������ƣ����Ը��ݴ������ҵ���
		_tstring Name;

		// ϵͳ�ڴ��еĸ�����
		// ���ڶ��㣨�����������Ƿ��͸�ʽ�������ʽ���û����壩��
		// ����ʹ��Blob��������ʾ���û�ʹ��ʱ�轫��ת��Ϊ�ʵ�����
		ComPtr<ID3DBlob> VertexBufferCPU;
		ComPtr<ID3DBlob> IndexBufferCPU;

		ComPtr<ID3D12Resource> VertexBufferGPU;
		ComPtr<ID3D12Resource> IndexBufferGPU;

		ComPtr<ID3D12Resource> VertexBufferUploader;
		ComPtr<ID3D12Resource> IndexBufferUploader;

		// �뻺������ص�����

		// �����ֽڿ��
		UINT VertexByteStride;

		// ���㻺�����ֽڴ�С
		UINT VertexBufferByteSize;

		// �����������ֽڴ�С
		UINT IndexBufferByteSize;

		// ������ʽ
		DXGI_FORMAT IndexFormat;

		// һ�� MeshGeometry �ṹ���ܹ��洢һ�鶥��/�����������еĶ��������
		// �������������������������񼸺��壬���ܵ������Ƴ����е������񣨵��������壩
		//map<_tstring, SubmeshGeometry> DrawArgs;
		std::unordered_map<_tstring, SubmeshGeometry> DrawArgs;

		// ���㻺������ͼ
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;

		// ������������ͼ
		D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

		void DisposeUploader();

		//@EndOf(MeshGeometry)
	};

}//DX12

// ����������

