#include "stdafx.h"
#include "DXUtil.h"

namespace dx12
{
	//static
	const XMFLOAT4X4 DXUtil::Identity4x4 = 
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};



	//static
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

	//static
	void DXUtil::GetHardwareAdapter(_In_ IDXGIFactory2* pFactory,
		_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter)
	{
		ComPtr<IDXGIAdapter1> adapter;
		*ppAdapter = nullptr;

		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see if the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}

		*ppAdapter = adapter.Detach();
	}

	//static
	ComPtr<ID3DBlob> DXUtil::LoadBinary(LPCTSTR filename)
	{
		std::ifstream fin(filename, std::ios::binary);

		fin.seekg(0, std::ios_base::end);
		std::ifstream::pos_type size = (int)fin.tellg();	// �ļ�����
		fin.seekg(0, std::ios_base::beg);

		ComPtr<ID3DBlob> blob;
		ThrowIfFailed(D3DCreateBlob((SIZE_T)size, blob.GetAddressOf()));

		fin.read((char*)blob->GetBufferPointer(), size);
		fin.close();

		return blob;
	}

	// static
	// filename		- hlsl �ļ���
	// defines		- �궨��(����ΪNULL)
	// entrypoint	- ��ڵ㣺"VSMain"��"PSMain"
	// target		- ��ɫ��ģ�ͣ�"vs_5_0"��"ps_5_0"
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
		std::wstring ws = KString::WString(filename.c_str());
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
	ComPtr<ID3D12Resource> DXUtil::CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		LONG_PTR byteSize,
		ComPtr<ID3D12Resource>& uploadBuffer)
	{
		// Ĭ�ϻ�����
		ComPtr<ID3D12Resource> defaultBuffer;	// GPU?
		 
		// Create the actual default buffer resource.
		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),	// Ĭ�϶�
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&defaultBuffer)));

		// In order to copy CPU memory data into our default buffer, we need to create
		// an intermediate upload heap. 
		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),	// �ϴ���
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


	//static
	UINT DXUtil::CalculateConstantBufferByteSize(UINT nSizeOfCBStruct)
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
		return ((nSizeOfCBStruct + 255) & (~255));
	}

	//static
	// ������ת�����ѿ�������
	XMFLOAT3 DXUtil::SphericalToCartesian(float radius, float theta, float phi)
	{
		return XMFLOAT3(
			radius*sinf(phi)*cosf(theta),	// x
			radius*cosf(phi),				// y
			radius*sinf(phi)*sinf(theta));	// z
	}

	//static
	XMMATRIX DXUtil::InverseTranspose(CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		XMMATRIX _Ty = M;
		_Ty.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR det = XMMatrixDeterminant(_Ty);
		return XMMatrixTranspose(XMMatrixInverse(&det, _Ty));
	}


	//static �����귽λ��[0, 2*PI)
	// x - �ѿ���x����
	// y - �ѿ���y����
	float DXUtil::Azimuth(float x, float y)
	{
		float theta = 0.0f;

		// Quadrant I or IV (1,4����)
		if (x >= 0.0f)
		{
			//If X = 0, then atanf(Y/X) = +pi/2 if Y > 0
			//               atanf(Y/X) = -pi/2 if Y < 0

			theta = atanf(y / x); // in [-pi/2, +pi/2]

			if (theta < 0.0f)
			{
				theta += 2.0f*(float)KMaths::PI; // in [0, 2*pi).
			}
		}
		// Quadrant II or III(2,3����)
		else
		{
			theta = atanf(y / x) + (float)KMaths::PI; // in [0, 2*pi).
		}

		return theta;
	}

	//static
	// �����λ��άʸ��
	XMVECTOR DXUtil::RandUnitVec3()
	{
		XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		XMVECTOR Zero = XMVectorZero();

		// Keep trying until we get a point on/in the hemisphere.
		while (true)
		{
			// Generate random point in the cube [-1,1]^3.
			XMVECTOR v = XMVectorSet(KMaths::Rand(-1.0f, 1.0f),
				KMaths::Rand(-1.0f, 1.0f),
				KMaths::Rand(-1.0f, 1.0f), 0.0f);

			// Ignore points outside the unit sphere in order to get an even distribution 
			// over the unit sphere.  Otherwise points will clump more on the sphere near 
			// the corners of the cube.

			if (XMVector3Greater(XMVector3LengthSq(v), One))
				continue;

			return XMVector3Normalize(v);
		}
	}

	// ?
	XMVECTOR DXUtil::RandHemisphereUnitVec3(XMVECTOR n)
	{
		XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		XMVECTOR Zero = XMVectorZero();

		// Keep trying until we get a point on/in the hemisphere.
		while (true)
		{
			// Generate random point in the cube [-1,1]^3.
			XMVECTOR v = XMVectorSet(KMaths::Rand(-1.0f, 1.0f),
				KMaths::Rand(-1.0f, 1.0f),
				KMaths::Rand(-1.0f, 1.0f), 0.0f);

			// Ignore points outside the unit sphere in order to get an even distribution 
			// over the unit sphere.  Otherwise points will clump more on the sphere near 
			// the corners of the cube.

			if (XMVector3Greater(XMVector3LengthSq(v), One))
				continue;

			// Ignore points in the bottom hemisphere.
			if (XMVector3Less(XMVector3Dot(n, v), Zero))
				continue;

			return XMVector3Normalize(v);
		}
	}







}//DX12
