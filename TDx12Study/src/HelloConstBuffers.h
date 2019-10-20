#pragma once
#include "AstudyDemo.h"

class HelloConstBuffers : public AstudyDemo
{
public:
	HelloConstBuffers();
	~HelloConstBuffers();

protected:
	void BuildAssets() override;
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPipelineState();
	void BuildTriangleGeometry();
	void BuildVertexBuffer();
	void BuildConstantBufferAndView();


	void Update() override;
	void Draw() override;

private:
	// 在此定义私有资产
	struct SceneConstants
	{
		XMFLOAT4 offset;
	};
	SceneConstants m_SceneConstants;

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



	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
	ComPtr<ID3D12Resource> m_ConstantBuffer;
	UINT8* m_pCbvData;



	//@EndOf(HelloConstBuffers)
};

