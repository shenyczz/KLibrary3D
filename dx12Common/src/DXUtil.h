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

namespace dx12
{

	class DX12LIBCOMMON_EXPORTS_CLASS DXUtils : public dxCommon::Utils
	{
	private:
		DXUtils() = delete;
		~DXUtils() = delete;

	public:
		// �����־, ������ɫ������
		static UINT GetCompileFlags();

		// ȡ��Ӳ�������� _Use_decl_annotations_
		static void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory,
			_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter);

		// ���ļ��������ݣ����磺��ɫ���ֽ��룩
		static ComPtr<ID3DBlob> LoadBinary(LPCTSTR filename);

		// ������ɫ��
		static ComPtr<ID3DBlob> CompileShader(
			_tstring filename,
			const D3D_SHADER_MACRO* defines,	// һ��ȡ nullptr
			const std::string& entrypoint,
			const std::string& target);

		// ����Ĭ�ϻ�����
		static ComPtr<ID3D12Resource> CreateDefaultBuffer(
			ID3D12Device* device,
			ID3D12GraphicsCommandList* cmdList,
			const void* initData,
			LONG_PTR byteSize,
			ComPtr<ID3D12Resource>& uploadBuffer);


		// ���㳣���������ֽڴ�С��Ҫ��256�ֽڶ��룩
		static UINT CalculateConstantBufferByteSize(UINT nSizeOfCBStruct);

		// ������ת��λ�ѿ�������(r,theta,phi)
		// theta	- 
		// phi		- 
		static XMFLOAT3 SphericalToCartesian(float radius, float theta, float phi);

		// ��ת��
		static XMMATRIX InverseTranspose(CXMMATRIX M);

		// �����귽λ��[0, 2*PI)
		static float Azimuth(float x, float y);

		// �����λ��άʸ��
		static XMVECTOR RandUnitVec3();

		// ?
		// Hemisphere������
		static XMVECTOR RandHemisphereUnitVec3(XMVECTOR n);



		//@EndOf(DXUtil)
	};

}//DX12Library

