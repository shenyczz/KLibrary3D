#pragma once

// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().

class HelloConstBuffers : public Dx12Sample
{
public:
	HelloConstBuffers(KWindow* pWindow);
	~HelloConstBuffers() {}

private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	struct SceneConstantBuffer
	{
		XMFLOAT4 offset;
	};

protected:
	void InitializeAssets() override;
	void OnUpdate() override;
	void PopulateCommandList();

protected:
	ComPtr<ID3D12Resource> m_vertexBuffer;			// vb
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;	// vbv

	ComPtr<ID3D12Resource> m_constantBuffer;		// cb
	SceneConstantBuffer m_constantBufferData;
	UINT8* m_pCbvDataBegin;

};

