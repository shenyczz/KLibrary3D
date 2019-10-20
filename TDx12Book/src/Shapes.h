#pragma once

#include "FrameResourceShapes.h"

class Shapes : public Dx12Book
{
public:
	Shapes();
	~Shapes();

protected:
	void OnMouseDown(WPARAM wParam, int x, int y);
	void OnMouseUp(WPARAM wParam, int x, int y);
	void OnMouseMove(WPARAM wParam, int x, int y);

protected:
	void BuildAssets() override;
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPipelineState();
	void BuildShapesGeometry();
	void BuildRenderItems();
	void BuildFrameResources();
	void BuildConstantBufferAndView();


	void Resized() override;
	void UpdateProj();

	void Update() override;
	void UpdateCamera();
	void UpdateObjectCBs();
	void UpdatePassCBs();


	void Draw() override;
	void DrawRenderItems();

private:
	// �ڴ˶���˽���ʲ�
	bool m_IsWireframe;

	// �۲��
	XMFLOAT3 m_EyePos = { 0.0f, 0.0f, 0.0f };


	UINT m_PassCbvOffset = 0;

	// CBV��������
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap;

	ComPtr<ID3D12RootSignature> m_RootSignature;

	std::unordered_map<_astring, std::unique_ptr<MeshGeometry>> m_Geometries;

	std::vector<std::unique_ptr<FrameResourceShapes::RenderItem>> m_AllRitems;

private:
	int m_FrameResourceCount = 1;
	int m_FrameResourceIndex = 0;
	std::vector<std::unique_ptr<FrameResourceShapes>> m_FrameResources;

	auto CurrentFrameResource()
	{
		return m_FrameResources.size() == 0
			? nullptr
			: m_FrameResources[m_FrameResourceIndex].get();
	}

	void UpdateFrameResource()
	{
		// ��ǰ֡��Դ
		auto pFrameResource = CurrentFrameResource();

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

		// ��һ��֡��Դ
		m_FrameResourceIndex = (m_FrameResourceIndex + 1) % m_FrameResourceCount;
	}


	//@EndOf(Shapes)
};

