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
#include <fstream>

namespace DX12
{
	class DX12BASE_EXPORTS_CLASS DXUtil
	{
	public:
		static UINT GetCompileFlags();

		// CB size is required to be 256-byte aligned.
		static UINT CalcConstantBufferByteSize(UINT byteSize);
		static UINT CalcConstantBufferByteSize(ComPtr<ID3D12Resource> ConstantBuffer);

		static ComPtr<ID3DBlob> CompileShader(
			_tstring& filename,
			const D3D_SHADER_MACRO* defines,
			const std::string& entrypoint,
			const std::string& target);

		static ComPtr<ID3DBlob> LoadBinary(LPCTSTR filename);
		//{
		//	std::ifstream fin(filename, std::ios::binary);

		//	fin.seekg(0, std::ios_base::end);
		//	std::ifstream::pos_type size = (int)fin.tellg();
		//	fin.seekg(0, std::ios_base::beg);

		//	ComPtr<ID3DBlob> blob;
		//	ThrowIfFailed(D3DCreateBlob((SIZE_T)size, blob.GetAddressOf()));

		//	fin.read((char*)blob->GetBufferPointer(), size);
		//	fin.close();

		//	return blob;
		//}

	private:
		DXUtil();
		~DXUtil();
	};

}//DX12
