#pragma once


enum class AdemoType
{
	TRI,
	QUAD,
	TRICB,
	TRIUV,
	BOX,
};

class Ademo : public Dx12Sample
{
public:
	Ademo();
	~Ademo() {}

protected:
	void OnResize() override;
	void OnUpdate() override;

	void OnMouseDown(WPARAM wParam, int x, int y) override;
	void OnMouseUp(WPARAM wParam, int x, int y) override;
	void OnMouseMove(WPARAM wParam, int x, int y) override;
	void OnMouseWheel(WPARAM wParam, LPARAM lParam) override;

protected:
	void BuildRootSignature() override;

	void BuildShadersAndInputLayout() override;
	void BuildPipelineStateObject() override;

	void BuildMeshData() override;

	void BuildConstantBufferAndView() override;
	void BuildTextureBufferAndView() override;

	void ReccorCommand() override;


private:
	struct _CB_TRI
	{
		XMFLOAT4 offset;
	};
	struct _CB_BOX
	{
		XMFLOAT4 offset;
		XMFLOAT4X4 wvp = DXUtil::Identity4x4;
	};

	//typedef _CB_TRI TCB;
	typedef _CB_BOX TCB;




	AdemoType m_demoType;

	MeshData m_mesh;

	TCB m_cbData;
	UINT8* m_pCbvDataBegin;	// ´«ËÍÖ¸Õë

	float m_Radius = 5.0f;
	float m_Theta = 1.5f*XM_PI;
	float m_Phi = XM_PIDIV4;

	POINT m_LastMousePos;



	//void RadarMesh(MeshData* pMesh);


	// DEFAULT
	void CreateRootSignatureDefault();
	void BuildPipelineStateObjectDefault();
	void InitVbIb(MeshData* pMesh);
	void BuildShadersAndInputLayoutFromFile(LPCTSTR lpszFileName);
	void BuildConstantBufferAndViewDefault();
	void ReccorCommandDefault();

	// TRI
	void BuildMeshData_tri();

	// QUAD
	void BuildMeshData_quad();

	// TRICB
	void CreateRootSignature_tricb();
	void BuildMeshData_tricb();
	void ReccorCommand_tricb();
	void Update_tricb();

	// TRIUV
	static const UINT TextureWidth = 256;
	static const UINT TextureHeight = 256;
	static const UINT TexturePixelSize = 4; // RGBA

	std::vector<UINT8> GenerateTextureData();

	void CreateRootSignature_triuv();
	void BuildMeshData_triuv();
	void BuildTextureBufferAndView_triuv();
	void ReccorCommand_triuv();

	// BOX
	void CreateRootSignature_box();
	void BuildMeshData_box();
	void ReccorCommand_box();
	void Update_box();


};

