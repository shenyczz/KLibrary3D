#include "stdafx.h"
#include "DXUtil.h"

namespace DX12 { DXUtil::DXUtil() {}	DXUtil::~DXUtil() {} }


namespace DX12
{

	UINT DXUtil::GetCompileFlags()
	{
		UINT compileFlags = 0;
#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		compileFlags = 0;
#endif
		return compileFlags;
	}

	// ComPtr<ID3D12Resource> m_ConstantBuffer
	UINT DXUtil::CalcConstantBufferByteSize(UINT byteSize)
	{
		// 常量缓冲区必须是最小硬件分配大小（通常为256字节）的倍数。
		// 因此四舍五入到最接近256的倍数。
		// 我们通过添加255来执行此操作，然后掩蔽存储所有位<256的较低2个字节。
		// Constant buffers must be a multiple of the minimum hardware
		// allocation size (usually 256 bytes).  So round up to nearest
		// multiple of 256.  We do this by adding 255 and then masking off
		// the lower 2 bytes which store all bits < 256.
		// Example: Suppose byteSize = 300.
		// (300 + 255) & ~255
		// 555 & ~255
		// 0x022B & ~0x00ff
		// 0x022B & 0xff00
		// 0x0200
		// 512
		return (byteSize + 255) & ~255;
	}
	UINT DXUtil::CalcConstantBufferByteSize(ComPtr<ID3D12Resource> ConstantBuffer)
	{
		return (sizeof(ConstantBuffer) + 255) & ~255;
	}


	/*
	
	HRESULT WINAPI
	D3DCompileFromFile(_In_ LPCWSTR pFileName,
	_In_reads_opt_(_Inexpressible_(pDefines->Name != NULL)) CONST D3D_SHADER_MACRO* pDefines,
	_In_opt_ ID3DInclude* pInclude,
	_In_ LPCSTR pEntrypoint,
	_In_ LPCSTR pTarget,
	_In_ UINT Flags1,
	_In_ UINT Flags2,
	_Out_ ID3DBlob** ppCode,
	_Always_(_Outptr_opt_result_maybenull_) ID3DBlob** ppErrorMsgs);

	*/
	ComPtr<ID3DBlob> DXUtil::CompileShader(
		_tstring& filename,
		const D3D_SHADER_MACRO* defines,	// 宏定义, 可空
		const std::string& entrypoint,		// 入口：比如 "VS"
		const std::string& target)			// 版本：比如 "vs_5_0"
	{
		UINT compileFlags = GetCompileFlags();

		HRESULT hr = S_OK;
		ComPtr<ID3DBlob> errors = nullptr;
		ComPtr<ID3DBlob> byteCode = nullptr;

		std::wstring ws = KUtil::WString(filename.c_str());
		LPCWSTR lpszwFilename = ws.c_str();
		hr = D3DCompileFromFile(lpszwFilename,
			defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint.c_str(),
			target.c_str(),
			compileFlags, 0, &byteCode, &errors);

		//ThrowIfFailed(D3DCompileFromFile(filePath.c_str(), nullptr, nullptr,
		//	"VSMain", "vs_5_0", compileFlags, 0, &m_VSByteCode, nullptr));

		if (errors != nullptr)
			OutputDebugStringA((char*)errors->GetBufferPointer());

		ThrowIfFailed(hr);

		return byteCode;
	}



	//static
	ComPtr<ID3DBlob> DXUtil::LoadBinary(LPCTSTR filename)
	{
		std::ifstream fin(filename, std::ios::binary);

		fin.seekg(0, std::ios_base::end);
		std::ifstream::pos_type size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);

		ComPtr<ID3DBlob> blob;
		ThrowIfFailed(D3DCreateBlob((SIZE_T)size, blob.GetAddressOf()));

		fin.read((char*)blob->GetBufferPointer(), size);
		fin.close();

		return blob;
	}




}//DX12
