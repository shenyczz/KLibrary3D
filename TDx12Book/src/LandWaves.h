#pragma once

#include "Waves.h"
#include "FrameResourceLaw.h"


class LandWaves : public Dx12Book
{
public:
	LandWaves();
	~LandWaves();

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
	void BuildRenderItems();
	void BuildFrameResources();

	void Resized() override;


	void Update() override;
	void UpdateCamera();
	void UpdateObjectCBs();
	void UpdateMainPassCB();
	void UpdateWaves();

	void Draw() override;
	void DrawRenderItems();




	float GetHillsHeight(float x, float z)const;
	XMFLOAT3 GetHillsNormal(float x, float z)const;

private:
	// 根签名
	ComPtr<ID3D12RootSignature> m_RootSignature;

	int m_FrameResourceCount;
	std::vector<std::unique_ptr<FrameResourceLaw>> m_FrameResources;
	FrameResourceLaw* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	bool m_IsWireframe = false;

	XMFLOAT3 m_EyePos = { 0.0f, 0.0f, 0.0f };

	std::unique_ptr<Waves> m_Waves;
	std::vector<std::unique_ptr<FrameResourceLaw::RenderItem>> m_AllRitems;

	// 设置动态顶点使用
	FrameResourceLaw::RenderItem* m_WavesRitem = nullptr;

	std::unordered_map<_astring, std::unique_ptr<MeshGeometry>> m_Geometries;



	//@EndOf(LandWaves)
};

