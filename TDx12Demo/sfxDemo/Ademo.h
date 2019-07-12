#pragma once



class Ademo : public Dx12Sample
{
public:
	Ademo();
	~Ademo();

protected:
	void InitializeGraphics() override;
	void InitializeAssets() override;

	void WindowResized() override;
	void UpdateParameter() override;
	void PopulateCommandList() override;

	void CreateRootSignature() override;

	void BuildByteCode() override;
	void BuildInputElementDescrips() override;
	void BuildPipelineStateObject() override;

	void BuildVertexBufferAndView() override;
	void BuildIndexBufferAndView() override;
	void BuildTextureBufferAndView() override;
	void BuildConstantBufferAndView() override;



};

