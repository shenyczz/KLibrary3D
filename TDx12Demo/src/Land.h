#pragma once

#include "FrameResourceLand.h"
#include "Waves.h"

class Land : public Dx12Book2
{
public:
	Land();
	~Land();

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
	void BuildPipelineStateObject() override;
	void BuildMeshData() override;
	void BuildOthers() override;

	void BuildCommandList() override;

private:
	void UpdatePSO();
	void UpdateProj();
	void UpdateCamera();
	void UpdateObjectCBs();
	void UpdatePassCBs();
	void UpdateWaves();

	void BuildLandGeometry();
	void BuildWavesGeometry();
	void BuildRenderItems();
	void BuildFrameResources();


	void DrawRenderItems();


	float GetHillsHeight(float x, float z) const;		// 山丘高度
	XMFLOAT3 GetHillsNormal(float x, float z) const;	// 山丘法线



private:
	bool m_IsWireframe;
	_astring m_meshName;

	unordered_map<PsoStyle, ComPtr<ID3D12PipelineState>> m_PSOs;

	unordered_map<_astring, unique_ptr<MeshGeometry>> m_Geometries;

	// 渲染项目
	vector<unique_ptr<RenderItem>> m_AllRitems;

	// 波浪
	unique_ptr<Waves> m_Waves;

	RenderItem* m_WavesRitem = nullptr;

	// 渲染资源
	int m_FrameResourceIndex = 0;
	int m_FrameResourceCount = 1;
	vector<unique_ptr<FrameResourceLand>> m_FrameResources;

	// 循环帧资源
	void CycleFrameResources()
	{
		m_FrameResourceIndex = (m_FrameResourceIndex + 1) % m_FrameResourceCount;

		if (CurrentFrameResource()->FenceValue != 0 && m_Fence->GetCompletedValue() < CurrentFrameResource()->FenceValue)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(m_Fence->SetEventOnCompletion(CurrentFrameResource()->FenceValue, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	// 当前帧资源
	FrameResourceLand* CurrentFrameResource() const
	{
		return m_FrameResources[m_FrameResourceIndex].get();
	}


	// @EndOf(Land)
};

