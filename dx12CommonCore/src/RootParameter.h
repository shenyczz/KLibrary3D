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
/******************************************************************************
******************************************************************************/

#pragma once

namespace dx12
{

	// 根描述表、根常量、根描述符
	class DX12LIBCOMMONCORE_CLASS RootParameter
	{
	public:
		RootParameter();
		~RootParameter();


	public:
		void Clear();
		const D3D12_ROOT_PARAMETER& operator () (void) const { return m_RootParam; }

	public:
		void InitAsConstants(
			UINT Num32BitValues,
			UINT Register,
			UINT Space = 0,
			D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);

		void InitAsConstantBufferView(
			UINT Register,
			UINT Space = 0,
			D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);

		void InitAsShaderResourceView(
			UINT Register,
			UINT Space = 0,
			D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);

		void InitAsUnorderedAccessView(
			UINT Register,
			UINT Space = 0,
			D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);

		void InitAsDescriptorTable(
			UINT numDescriptorRanges,
			const D3D12_DESCRIPTOR_RANGE* pDescriptorRanges,
			D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);

	private:
		D3D12_ROOT_PARAMETER m_RootParam;

		//@EndOf(KRootParameter)
	};


}

/*

struct D3D12_ROOT_PARAMETER
{
    D3D12_ROOT_PARAMETER_TYPE ParameterType;
    union 
    {
        D3D12_ROOT_DESCRIPTOR_TABLE DescriptorTable;
        D3D12_ROOT_CONSTANTS Constants;
        D3D12_ROOT_DESCRIPTOR Descriptor;
    } ;
    D3D12_SHADER_VISIBILITY ShaderVisibility;
};

*/