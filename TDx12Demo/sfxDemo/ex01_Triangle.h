#pragma once


class Triangle : public Dx12Sample
{
public:
	Triangle();
	~Triangle();

private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
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

	//void BuildVertexBufferAndView();

};

