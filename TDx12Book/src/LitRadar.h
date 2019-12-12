#pragma once

#include "FrameResourceLir.h"

using namespace std;

struct XINFO
{
	int dims[3];
	float elementSize[3];
	float elememtSpace[3];

	float lon0;
	float lat0;
	float xInterval;
	float yInterval;
};

struct XYZ
{
	float x, y, z;

};

struct RCOLOR
{
	float r, g, b, a;
};

struct IsoSuface
{
	float EValue;
	RCOLOR Color;
	std::vector<XYZ> Positions;
	std::vector<XYZ> Normals;
	std::vector<uint32> Indices;
};


enum class PSO
{
	Default = 0,
	Solid,
	Wireframe,
};

enum class RLayer
{
	All = 0,

	RL050 = 50,
	RL100 = 100,
	RL150 = 150,
	RL200 = 200,
	RL250 = 250,
	RL300 = 300,
	RL350 = 350,
	RL400 = 400,
	RL450 = 450,
	RL500 = 500,
	RL550 = 550,
	RL600 = 600,
	RL650 = 650,
};

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
	void BuildPipelineStateSub(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso);
	void BuildRadarGeometry_0();
	void BuildRadarGeometry();
	void BuildMaterials_0();
	void BuildMaterials();
	void BuildMaterials(int& index, RLayer eLayer);
	void BuildRenderItems_0();
	void BuildRenderItems();
	void BuildRenderItems(int& index, RLayer eLayer);
	void BuildFrameResources();

	void Resized() override;

	void Update() override;
	void UpdateCamera();
	void UpdateObjectCB();
	void UpdatePassCB();
	void UpdateMaterialCB();

	void Draw() override;
	void DrawRenderItems_0();
	void DrawRenderItems(ID3D12GraphicsCommandList* pCommandList, const std::vector<FrameResourceLir::RenderItem*>& ritems);

	void DoAppIdle() override;


private:
	bool LoadSlicerFromFiles16(_tstring filePrefix, int indexBeg, int indexEnd, int width, int height);

	bool LoadSlicerFromFiles16(_tstring fileName);
	void LoadSlicerFromFiles16_mhd(_tstring fileNam);
	void LoadSlicerFromFiles16_raw(_tstring fileName);

	// 雷达三维重构
	void RebuildRadar3d(vector<TSlice<uint16>>* pSlices, const vector<double>& contourValues);
	void GenerateMeshData(MeshData* pMeshData);

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

	unordered_map<RLayer, unique_ptr<MeshData>> m_MeshDatas;


	unordered_map<RLayer, unique_ptr<MeshGeometry>> m_Geometries;
	unordered_map<RLayer, unique_ptr<Material>> m_Materials;

	unordered_map<RLayer, _astring> m_MatNames;			// 材质名称
	unordered_map<RLayer, XMFLOAT4> m_DiffuseAlbedos;	// 反照率


	vector<unique_ptr<FrameResourceLir::RenderItem>> m_AllRitems;
	vector<FrameResourceLir::RenderItem*> m_RitemLayer[(int)FrameResourceLir::RenderLayer::Count];

	// 观察点
	XMFLOAT3 m_EyePos = { 0.0f, 0.0f, 0.0f };

	// 保存雷达切片数据
	vector<TSlice<uint16>>	m_Slices;
	KMarchingCubes m_KMarchingCubes;



private:

	XINFO m_xinfo;


	//@EndOf(AbookDemo)
};

