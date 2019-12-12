/******************************************************************************
*
* Usage:
*
* [01]������ Ademo, ����Ϊ Dx12Sample
* [02]�޸Ĺ��캯��Ϊ�������캯������Ϊ���� Dx12Sample Ĭ�Ϲ��캯����ɾ��
* [03]�������캯��Ҫ���л���Ĳ������캯��
* [04]�������º���
*
*	void BuildAssets() override;
*	void Resized() override;
*	void Update() override;
*	void Draw() override;
*
*	void DoAppIdle() override;	// ��Ҫʱ����
*
* [05]��Ҫʱ�������º���
*	// ����
*	void OnInit() override;
*	void OnResize() override;
*	void OnUpdate() override;
*	void OnRender() override;
*
*	// �����Ϣ����
*	void OnMouseDown(WPARAM wParam, int x, int y) override;
*	void OnMouseUp(WPARAM wParam, int x, int y) override;
*	void OnMouseMove(WPARAM wParam, int x, int y) override;
*
******************************************************************************/

#pragma once

class Abook12Demo : public Dx12Book
{
public:
	Abook12Demo();
	~Abook12Demo();

protected:
	void OnMouseDown(WPARAM wParam, int x, int y);
	void OnMouseUp(WPARAM wParam, int x, int y);
	void OnMouseMove(WPARAM wParam, int x, int y);

protected:
	void BuildAssets() override;
	//void BuildRootSignature();
	//void BuildShadersAndInputLayout();
	//void BuildPipelineState();
	//void BuildBoxGeometry();
	//void BuildConstantBufferAndView();

	void Resized() override;

	void Update() override;
	//void UpdateCamera();
	//void UpdateObjectCBs();
	//void UpdatePassCB();

	void Draw() override;
	//void DrawRenderItems();

private:
	// �ڴ˶���˽���ʲ�
	bool m_IsWireframe;

	ComPtr<ID3D12RootSignature> m_RootSignature;

	std::unordered_map<_astring, std::unique_ptr<MeshGeometry>> m_Geometries;

	//@EndOf(AbookDemo)
};

