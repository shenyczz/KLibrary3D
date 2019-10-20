#pragma once

class AstudyDemo : public Dx12Study
{
public:
	AstudyDemo();
	~AstudyDemo();

protected:
	void OnMouseDown(WPARAM wParam, int x, int y);
	void OnMouseUp(WPARAM wParam, int x, int y);
	void OnMouseMove(WPARAM wParam, int x, int y);

protected:
	void BuildAssets() override;
	void Resized() override
	{
		// ����ͶӰ����
		XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*XM_PI, AspectRatio(), 1.0f, 1000.0f);
		XMStoreFloat4x4(&m_Proj, P);
	}
	void Update() override;
	void Draw() override;



protected:
	// �ڴ˶���˽���ʲ�
	bool m_IsWireframe;
	ComPtr<ID3D12RootSignature> m_RootSignature;

	//@EndOf(AstudyDemo)
};

