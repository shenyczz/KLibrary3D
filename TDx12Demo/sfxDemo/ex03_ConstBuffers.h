#pragma once

class ConstBuffers : public Dx12Sample
{
public:
	ConstBuffers();
	~ConstBuffers();

private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	struct ConstantObject
	{
		XMFLOAT4 offset;
	};

protected:
	void InitializeGraphics() override;
	void InitializeAssets() override;

	void WindowResized() override;
	void UpdateParameter() override;
	void PopulateCommandList() override;

	void CreateRootSignature() override;

	void BuildByteCode() override;
	void BuildVertexBufferAndView() override;
	void BuildIndexBufferAndView() override;
	void BuildConstantBufferAndView() override;
	void BuildInputElementDescrips() override;
	void BuildPipelineStateObject() override;

private:
	ComPtr<ID3D12Resource> m_ConstantBuffer;		// Constant Buffer
	UINT8* m_pCbvDataBegin;


	ConstantObject m_ConstantObject;				// 



	//@EndOf(ConstBuffers)
};

