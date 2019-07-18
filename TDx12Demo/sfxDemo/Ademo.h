#pragma once


enum class AdemoType
{
	TRI,
	QUAD,
	TRICB,
	TRIUV,
};

class Ademo : public Dx12Sample
{
public:
	Ademo();
	~Ademo();

protected:
	void Resize() override;
	void Update() override;


	void CreateRootSignature() override;

	void BuildShadersAndInputLayout() override;
	void BuildPipelineStateObject() override;

	void BuildMeshData() override;

	void BuildConstantBufferAndView() override;
	void BuildTextureBufferAndView() override;

	void ReccorCommand() override;

private:
	AdemoType m_demoType;

	MeshData m_mesh;


	// DEFAULT
	void CreateRootSignatureDefault();
	void BuildPipelineStateObjectDefault();
	void BuildShadersAndInputLayoutFromFile(LPCTSTR lpszFileName);

	// TRI
	void CreateRootSignature_tri();				// default
	void BuildShadersAndInputLayout_tri();
	void BuildPipelineStateObject_tri();		// default
	void BuildMeshData_tri();
	void ReccorCommand_tri();

	// QUAD
	void CreateRootSignature_quad();			// default
	void BuildShadersAndInputLayout_quad();
	void BuildPipelineStateObject_quad();		// default
	void BuildMeshData_quad();
	void ReccorCommand_quad();

	// TRICB
	struct TCB
	{
		XMFLOAT4 offset;
	};

	TCB m_cbData;
	UINT8* m_pCbvDataBegin;

	void CreateRootSignature_tricb();
	void BuildShadersAndInputLayout_tricb();
	void BuildPipelineStateObject_tricb();		// default
	void BuildMeshData_tricb();
	void BuildConstantBufferAndView_tricb();
	void ReccorCommand_tricb();
	void Update_tricb();

	// TRIUV
	static const UINT TextureWidth = 256;
	static const UINT TextureHeight = 256;
	static const UINT TexturePixelSize = 4; // RGBA

	std::vector<UINT8> GenerateTextureData();

	void CreateRootSignature_triuv();
	void BuildShadersAndInputLayout_triuv();
	void BuildPipelineStateObject_triuv();		// default
	void BuildMeshData_triuv();
	void BuildTextureBufferAndView_triuv();
	void ReccorCommand_triuv();




};

