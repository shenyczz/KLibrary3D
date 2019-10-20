#pragma once

#include "AstudyDemo.h"

class HelloTriangle : public AstudyDemo
{
public:
	HelloTriangle();
	~HelloTriangle();

protected:
	void BuildAssets() override;
	void BuildBundle();
	void BuildRootSignature();
	void BuildRootSignatureCB();
	void BuildShadersAndInputLayout();
	void BuildPipelineState();
	void BuildTriangleGeometry();
	void BuildVertexBuffer();

	void Resized() override
	{
		// 构建投影矩阵
		XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
		XMStoreFloat4x4(&m_Proj, P);
	}
	void Update() override;
	void Draw() override;

private:
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()
	{
		return D3D12_VERTEX_BUFFER_VIEW
		{
			m_VertexBuffer->GetGPUVirtualAddress(),
			m_Triangle.vbSizeInBytes(),
			m_Triangle.vbStrideInBytes()
		};
	}


private:
	// 在此定义私有资产
	MeshData m_Triangle;
	ComPtr<ID3D12Resource> m_VertexBuffer;

	ComPtr<ID3D12CommandAllocator> m_BundleAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_Bundle;

	//@EndOf(AstudyDemo)
};

