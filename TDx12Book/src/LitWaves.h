#pragma once

#include "Waves.h"
#include "FrameResourceLiw.h"

using namespace std;

class LitWaves : public Dx12Book
{
public:
	LitWaves();
	~LitWaves();

protected:
	void OnMouseDown(WPARAM wParam, int x, int y);
	void OnMouseUp(WPARAM wParam, int x, int y);
	void OnMouseMove(WPARAM wParam, int x, int y);

private:
	void BuildAssets() override;
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPipelineState();
	void BuildLandGeometry();
	void BuildWavesGeometry();
	void BuildMaterials();
	void BuildRenderItems();
	void BuildFrameResources();


	void Resized() override;


	void Update() override;
	void UpdateCamera();
	void UpdateObjectCBs();
	void UpdateMaterialCBs();
	void UpdatePassCB();
	void UpdateWaves();


	void Draw() override;
	void DrawRenderItems();





	void OnKeyboardInput();

	float GetHillsHeight(float x, float z) const;
	XMFLOAT3 GetHillsNormal(float x, float z) const;

private:
	int m_FrameResourceCount = 3;
	int m_FrameResourceIndex = 0;
	std::vector<std::unique_ptr<FrameResourceLiw>> m_FrameResources;

	void CircularFrameResource()
	{
		m_FrameResourceIndex = (m_FrameResourceIndex + 1) % m_FrameResourceCount;
		FrameResourceLiw* pFrameResource = CurrentFrameResource();

		// Has the GPU finished processing the commands of the current frame resource?
		// If not, wait until the GPU has completed commands up to this fence point.
		if (pFrameResource != nullptr
			&& pFrameResource->FenceValue != 0
			&& pFrameResource->FenceValue > m_Fence->GetCompletedValue())
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(m_Fence->SetEventOnCompletion(CurrentFrameResource()->FenceValue, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	FrameResourceLiw* CurrentFrameResource()
	{
		return m_FrameResources.size() == 0
			? nullptr
			: m_FrameResources[m_FrameResourceIndex].get();
	}


private:
	bool m_IsWireframe;

	ComPtr<ID3D12RootSignature> m_RootSignature;

	unordered_map<_astring, unique_ptr<Material>> m_Materials;
	unordered_map<_astring, unique_ptr<MeshGeometry>> m_Geometries;

	unique_ptr<Waves> m_Waves;
	FrameResourceLiw::RenderItem* m_WavesRitem;

	vector<unique_ptr<FrameResourceLiw::RenderItem>> m_AllRitems;



	// ¹Û²ìµã
	XMFLOAT3 m_EyePos = { 0.0f, 0.0f, 0.0f };

	float m_SunTheta = 1.25f*XM_PI;
	float m_SunPhi = XM_PIDIV4;

	//@EndOf(LitWaves)
};

