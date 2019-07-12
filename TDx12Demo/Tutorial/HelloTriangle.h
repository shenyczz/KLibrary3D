#pragma once

class HelloTriangle : public Dx12Sample
{
public:
	HelloTriangle(KWindow* pWindow);
	~HelloTriangle();

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

protected:
	virtual void InitializeAssets() override;
	virtual void BuildPipelineStateObject() override;
	virtual void PopulateCommandList() override;

private:
	void CreateVertexBuffer();

private:
	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;


};

