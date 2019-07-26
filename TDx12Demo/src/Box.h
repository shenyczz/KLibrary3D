#pragma once

#include "FrameResource.h"

class Box : public Dx12Book
{
public:
	Box();
	~Box();

protected:
	void OnResize() override;
	void OnUpdate() override;

protected:
	void OnMouseDown(WPARAM wParam, int x, int y) override;


protected:
	void BuildRootSignature() override;

	void BuildShadersAndInputLayout() override;
	void BuildPipelineStateObject() override;

	void BuildMeshData() override;

	void BuildConstantBufferAndView() override;
	void BuildTextureBufferAndView() override {}

	void ReccorCommand() override;

private:
	unordered_map<PsoStyle, ComPtr<ID3D12PipelineState>> m_psoMap;

	unordered_map<_astring, unique_ptr<MeshGeometry>> m_Geometries;
	unique_ptr<TUploadBuffer<FrameResourceBox::ObjectConstants>> m_ObjectCB;




	//MeshData m_mesh;
	void RadarMesh(MeshData* pMesh);

	// PSO

	bool m_bWireFrame;

};

