#pragma once

class Box : public Dx12Book
{
public:
	Box();
	~Box();

	struct ObjectConstants
	{
		XMFLOAT4X4 WorldViewProj = DXUtil::Identity4x4;
	};

protected:
	void OnMouseDown(WPARAM wParam, int x, int y);
	void OnMouseUp(WPARAM wParam, int x, int y);
	void OnMouseMove(WPARAM wParam, int x, int y);

protected:
	void BuildAssets() override;
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPipelineState();
	void BuildConstantBufferAndView();
	void BuildBoxGeometry();
	void BuildCarGeometry();
	void BuildSkullGeometry();
	void BuildShapesGeometry();



	void Resized() override;

	void Update() override;
	void UpdateCamera();
	void UpdateObjectCBs();


	void Draw() override;


	void BuildBoxRadar();
	//void RadarMesh0(MeshData* pMesh);
	//void RadarMesh1(MeshData* pMesh);
	//void RadarMesh2(MeshData* pMesh);
	//void RadarMesh3(MeshData* pMesh);
	//void GetRadarColor(float v, float& r, float& g, float& b);



private:
	// 在此定义私有资产
	bool m_IsWireframe;

	int m_GeoFlag;

	ComPtr<ID3D12RootSignature> m_RootSignature;

	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
	std::unique_ptr<TUploadBuffer<ObjectConstants>> m_ObjectCB;

	std::unordered_map<_astring, std::unique_ptr<MeshGeometry>> m_Geometries;

	// 观察点
	XMFLOAT3 m_EyePos = { 0.0f, 0.0f, 0.0f };


	//@EndOf(Box)
};

