#include "stdafx.h"
#include "DXUtil.h"

namespace DX12 { DXUtil::DXUtil() {}	DXUtil::~DXUtil() {} }


namespace DX12
{

	// static
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

	// static
	UINT DXUtil::CalcConstantBufferByteSize(UINT nSizeOfCBStruct)
	{
		// ������������������СӲ�������С��ͨ��Ϊ256�ֽڣ��ı�����
		// ����������뵽��ӽ�256�ı�����
		// ����ͨ�����255��ִ�д˲�����Ȼ���ڱδ洢����λ<256�Ľϵ�2���ֽڡ�
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
		return (nSizeOfCBStruct + 255) & ~255;
	}

	// static
	ComPtr<ID3DBlob> DXUtil::CompileShader(
		_tstring filename,
		const D3D_SHADER_MACRO* defines,	// �궨��, �ɿ�
		const std::string& entrypoint,		// ��ڣ����� "VS"
		const std::string& target)			// �汾������ "vs_5_0"
	{
		UINT compileFlags = GetCompileFlags();

		HRESULT hr = S_OK;
		ComPtr<ID3DBlob> errors = nullptr;
		ComPtr<ID3DBlob> byteCode = nullptr;

		// ���ļ������ UNICODE
		std::wstring ws = KUtil::WString(filename.c_str());
		LPCWSTR lpszwFilename = ws.c_str();

		hr = D3DCompileFromFile(lpszwFilename,	// hlsl �ļ���
			defines,							// nullptr
			D3D_COMPILE_STANDARD_FILE_INCLUDE,	// ? 1
			entrypoint.c_str(),					// "VSMain"
			target.c_str(),						// "vs_5_0"
			compileFlags,						// _In_ UINT Flags1
			0,									// _In_ UINT Flags2
			&byteCode,
			&errors);

		if (errors != nullptr)
		{
			OutputDebugStringA((char*)errors->GetBufferPointer());
		}

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

	// static
	ComPtr<ID3D12Resource> DXUtil::CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		LONG_PTR byteSize,
		ComPtr<ID3D12Resource>& uploadBuffer)
	{
		ComPtr<ID3D12Resource> defaultBuffer;
		 
		// Create the actual default buffer resource.
		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),	// Ĭ�϶�
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

		// In order to copy CPU memory data into our default buffer, we need to create
		// an intermediate upload heap. 
		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf())));


		// Describe the data we want to copy into the default buffer.
		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = initData;
		subResourceData.RowPitch = byteSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
		// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
		// the intermediate upload heap data will be copied to mBuffer.
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
		{
			UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
		}
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

		// Note: uploadBuffer has to be kept alive after the above function calls because
		// the command list has not been executed yet that performs the actual copy.
		// The caller can Release the uploadBuffer after it knows the copy has been executed.

		return defaultBuffer;
	}










}//DX12
