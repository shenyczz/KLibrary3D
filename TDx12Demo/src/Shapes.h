#pragma once

#include "FrameResourceShapes.h"

class Shapes : public Dx12Book
{
public:
	Shapes();
	~Shapes();

protected:
	void OnResize() override;
	void OnUpdate() override;
	void OnRender() override;

protected:
	void OnMouseDown(WPARAM wParam, int x, int y) override;
	void OnMouseUp(WPARAM wParam, int x, int y) override;
	void OnMouseMove(WPARAM wParam, int x, int y) override;


protected:
	void BuildRootSignature() override;
	void BuildShadersAndInputLayout() override;
	void BuildMeshData() override;
	void BuildConstantBufferAndView() override;
	void BuildTextureBufferAndView() override {}
	void BuildPipelineStateObject() override;

	void BuildCommandList() override;

private:
	void UpdateProj();
	void UpdateCamera();
	void UpdatePassCBs();
	void UpdateObjectCBs();

	void BuildBox2();
	void BuildShapesGeometry();
	void BuildRenderItems();
	void BuildFrameResources();

	void BuildConstantBuffer();
	void BuildConstantBufferView();


	void DrawMeshData();


private:
	bool m_IsWireframe;

	int m_FrameResourceCount;

	XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };


	std::vector<std::unique_ptr<FrameResourceShapes>> m_FrameResources;
	FrameResourceShapes* m_CurrFrameResource = nullptr;
	int m_CurrFrameResourceIndex = 0;

	unordered_map<_astring, unique_ptr<MeshGeometry>> m_Geometries;
	unordered_map<PsoStyle, ComPtr<ID3D12PipelineState>> m_psoMap;

	vector<unique_ptr<FrameResourceShapes::RenderItem>> m_AllRitems;

	// PassCBs ÆðÊ¼µØÖ·
	int m_PassCbvOffset = 0;


	//MeshData m_mesh;

	// PSO


};

