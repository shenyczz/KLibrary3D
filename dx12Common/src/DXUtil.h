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

	class DX12LIBCOMMON_EXPORTS_CLASS DXUtils : public dxCommon::Utils
	{
	private:
		DXUtils() = delete;
		~DXUtils() = delete;

	public:
		// 编译标志, 用于着色器编译
		static UINT GetCompileFlags();

		// 取得硬件适配器 _Use_decl_annotations_
		static void GetHardwareAdapter(_In_ IDXGIFactory2* pFactory,
			_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter);

		// 从文件读入数据（比如：着色器字节码）
		static ComPtr<ID3DBlob> LoadBinary(LPCTSTR filename);

		// 编译着色器
		static ComPtr<ID3DBlob> CompileShader(
			_tstring filename,
			const D3D_SHADER_MACRO* defines,	// 一般取 nullptr
			const std::string& entrypoint,
			const std::string& target);

		// 创建默认缓冲区
		static ComPtr<ID3D12Resource> CreateDefaultBuffer(
			ID3D12Device* device,
			ID3D12GraphicsCommandList* cmdList,
			const void* initData,
			LONG_PTR byteSize,
			ComPtr<ID3D12Resource>& uploadBuffer);


		// 计算常量缓冲区字节大小（要求256字节对齐）
		static UINT CalculateConstantBufferByteSize(UINT nSizeOfCBStruct);

		// 球坐标转换位笛卡尔坐标(r,theta,phi)
		// theta	- 
		// phi		- 
		static XMFLOAT3 SphericalToCartesian(float radius, float theta, float phi);

		// 反转置
		static XMMATRIX InverseTranspose(CXMMATRIX M);

		// 极坐标方位角[0, 2*PI)
		static float Azimuth(float x, float y);

		// 随机单位三维矢量
		static XMVECTOR RandUnitVec3();

		// ?
		// Hemisphere（半球）
		static XMVECTOR RandHemisphereUnitVec3(XMVECTOR n);



		//@EndOf(DXUtil)
	};

}//DX12Library

