#pragma once

#include "FrameResource.h"

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
	void BuildRootSignature() override;
	void BuildShadersAndInputLayout() override;
	void BuildPipelineStateObject() override;
	void BuildMeshData() override;
	void BuildConstantBufferAndView() override;

	void ReccorCommand() override;


private:
	void UpdateCamera();
	void UpdateObjectCBs();
	void UpdateMainPassCB();

	void BuiltdShapesGeometry();
	void BuildRenderItems();
	void BuildFrameResources();
	void BuildDescriptorHeaps();

	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<FrameResourceShapes::RenderItem*>& ritems);


private:
	bool m_IsWireframe = false;

	XMFLOAT3 m_EyePos = { 0.0f, 0.0f, 0.0f };

	UINT m_PassCbvOffset = 0;
	FrameResourceShapes::PassConstants mMainPassCB;
	//unique_ptr<TUploadBuffer<FrameResourceShapes::ObjectConstants>> m_ObjectCB;


	// Ö¡×ÊÔ´
	std::vector<std::unique_ptr<FrameResourceShapes>> m_FrameResources;
	FrameResourceShapes* m_CurrFrameResource = nullptr;
	int m_CurrFrameResourceIndex = 0;

	// äÖÈ¾Ïî
	std::vector<std::unique_ptr<FrameResourceShapes::RenderItem>> m_AllRitems;

	// Render items divided by PSO.
	std::vector<FrameResourceShapes::RenderItem*> m_OpaqueRitems;


	unordered_map<PsoStyle, ComPtr<ID3D12PipelineState>> m_psoMap;
	unordered_map<_astring, unique_ptr<MeshGeometry>> m_Geometries;

	//@EndOf(Shapes)
};

