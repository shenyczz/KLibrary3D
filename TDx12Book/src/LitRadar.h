#pragma once

#include "FrameResourceLir.h"

using namespace std;

class LitRadar : public Dx12Book
{
public:
	LitRadar();
	~LitRadar();

protected:
	void OnMouseDown(WPARAM wParam, int x, int y);
	void OnMouseUp(WPARAM wParam, int x, int y);
	void OnMouseMove(WPARAM wParam, int x, int y);
	void OnKeyboardInput();

protected:
	void BuildAssets() override;
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPipelineState();
	void BuildRadarGeometry();
	void BuildMaterials();
	void BuildRenderItems();
	void BuildFrameResources();

	void Resized() override;

	void Update() override;
	void UpdateCamera();
	void UpdateObjectCB();
	void UpdatePassCB();
	void UpdateMaterialCB();

	void Draw() override;
	void DrawRenderItems();

	void DoAppIdle() override;


private:
	bool LoadSlicerFromFiles16(_tstring filePrefix, int indexBeg, int indexEnd, int width, int height);

	bool LoadSlicerFromFiles16(_tstring fileName);
	void LoadSlicerFromFiles16_mhd(_tstring fileNam);
	void LoadSlicerFromFiles16_raw(_tstring fileName);

	// 雷达三维重构
	void RebuildRadar3d(vector<TSlice<uint16>>* pSlices, const vector<double>& contourValues);
	void GenerateMesh(MeshData* pMeshData);

	// 输出网格数据
	void OutputMesh(_tstring fileName, const MeshData* pMeshData);
	void InputMesh(_tstring fileName, MeshData* pMeshData);

	void GetRadarColor(float v, float& r, float& g, float& b);



private:
	int m_FrameResourceCount;
	int m_FrameResourceIndex;
	vector<unique_ptr<FrameResourceLir>> m_FrameResources;

	// 循环帧资源
	void CircularFrameResource()
	{
		m_FrameResourceIndex = (m_FrameResourceIndex + 1) % m_FrameResourceCount;
		FrameResourceLir* pFrameResource = CurrentFrameResource();

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

	// 当前帧资源
	FrameResourceLir* CurrentFrameResource()
	{
		return m_FrameResources.size() == 0
			? nullptr
			: m_FrameResources[m_FrameResourceIndex].get();
	}


private:
	ComPtr<ID3D12RootSignature> m_RootSignature;

	bool m_IsWireframe;
	vector<TSlice<uint16>>	m_Slices;

	unordered_map<_astring, unique_ptr<MeshGeometry>> m_Geometries;
	unordered_map<_astring, unique_ptr<Material>> m_Materials;

	vector<unique_ptr<FrameResourceLir::RenderItem>> m_AllRitems;

	// 观察点
	XMFLOAT3 m_EyePos = { 0.0f, 0.0f, 0.0f };

	//float m_SunTheta = XM_PI * 5.0f / 4;
	//float m_SunPhi = XM_PI * 1.0f / 4;

	KMarchingCubes m_KMarchingCubes;

private:
	struct XINFO
	{
		int dim[3];
		float elementSize[3];
		float elememtSpace[3];

		float lon0;
		float lat0;
		float xInterval;
		float yInterval;
	};

	XINFO m_xinfo;


	//@EndOf(AbookDemo)
};

