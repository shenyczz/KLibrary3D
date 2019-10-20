#pragma once
#include "AstudyDemo.h"

class HelloTexture : public AstudyDemo
{
public:
	HelloTexture();
	~HelloTexture();

protected:
	void BuildAssets() override;
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPipelineState();
	void BuildTriangleGeometry();
	void BuildVertexBuffer();
	void BuildTextureBufferAndView();

	//void BuildRootSignature2();
	//void BuildTextureBufferAndView2();




	void Resized() override
	{
		// 更行投影矩阵
		XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
		XMStoreFloat4x4(&m_Proj, P);
	}
	void Update() override;
	void Draw() override;


private:
	std::vector<UINT8> GenerateTextureData();

private:
	static const UINT TextureWidth = 256;
	static const UINT TextureHeight = 256;
	static const UINT TexturePixelSize = 4;	// The number of bytes used to represent a pixel in the texture.

	MeshData m_Triangle;

	ComPtr<ID3D12Resource> m_VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()
	{
		return D3D12_VERTEX_BUFFER_VIEW
		{
			m_VertexBuffer->GetGPUVirtualAddress(),
			m_Triangle.vbSizeInBytes(),
			m_Triangle.vbStrideInBytes()
		};
	}

	// 描述符堆
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;

	ComPtr<ID3D12Resource> m_Texture;
	ComPtr<ID3D12Resource> m_TextureUploadHeap;



	//@EndOf(HelloTexture)
};

