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

namespace DX12
{
	class DX12BASE_EXPORTS_CLASS DXUtil
	{
	private:
		DXUtil() = delete;
		~DXUtil() = delete;

		// �����־
		static UINT GetCompileFlags();

	public:

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

		// ������ת��λ�ѿ�������
		// theta	- 
		// phi		- 
		static XMVECTOR SphericalToCartesian(float radius, float theta, float phi);

		// ��ת��
		static XMMATRIX InverseTranspose(CXMMATRIX M);

		// �����귽λ��[0, 2*PI)
		static float Azimuth(float X, float Y);

		// ?
		static DirectX::XMVECTOR RandUnitVec3();

		// ?
		// Hemisphere������
		static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);






		// ����
		static const XMFLOAT4X4 Identity4x4;




		//@EndOf(DXUtil)
	};

}//DX12






extern const int gNumFrameResources;

namespace DX12
{

#define MaxLights 16

	struct Light
	{
		XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };		// ǿ��
		float FalloffStart = 1.0f;						// point/spot light only
		XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };		// directional/spot light only
		float FalloffEnd = 10.0f;						// point/spot light only
		XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };		// point/spot light only
		float SpotPower = 64.0f;						// spot light only
	};


	struct Texture
	{
		// Unique material name for lookup.
		_tstring Name = _T("");

		_tstring Filename = _T("");

		ComPtr<ID3D12Resource> Resource = nullptr;
		ComPtr<ID3D12Resource> UploadHeap = nullptr;
	};


	// Simple struct to represent a material for our demos.  A production 3D engine
	// would likely create a class hierarchy of Materials.
	struct Material
	{
		// Unique material name for lookup.
		_astring Name;

		// Index into constant buffer corresponding to this material.
		int MatCBIndex = -1;

		// Index into SRV heap for diffuse texture.
		int DiffuseSrvHeapIndex = -1;

		// Index into SRV heap for normal texture.
		int NormalSrvHeapIndex = -1;

		// Dirty flag indicating the material has changed and we need to update the constant buffer.
		// Because we have a material constant buffer for each FrameResource, we have to apply the
		// update to each FrameResource.  Thus, when we modify a material we should set 
		// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
		int NumFramesDirty = gNumFrameResources;

		// Material constant buffer data used for shading.
		XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = .25f;
		XMFLOAT4X4 MatTransform = DXUtil::Identity4x4;
	};

	struct MaterialConstants
	{
		XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };	// ����
		XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;

		// Used in texture mapping.
		XMFLOAT4X4 MatTransform = DXUtil::Identity4x4;
	};


}//namespace DX12
