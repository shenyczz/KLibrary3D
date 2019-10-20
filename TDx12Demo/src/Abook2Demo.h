#pragma once

class Abook2Demo : public Dx12Book2
{
public:
	Abook2Demo();
	~Abook2Demo();

protected:
	// ����
	void OnResize() override;
	void OnUpdate() override;
	void OnRender() override;

	// �����Ϣ����
	void OnMouseDown(WPARAM wParam, int x, int y) override;
	void OnMouseUp(WPARAM wParam, int x, int y) override;
	void OnMouseMove(WPARAM wParam, int x, int y) override;

	// BuildAssets()����
	void BuildRootSignature() override;
	void BuildShadersAndInputLayout() override;
	void BuildPipelineStateObject() override;
	void BuildMeshData() override;
	void BuildOthers() override;

	// OnRender()����
	void BuildCommandList() override;


private:
	// ���ﶨ��˽���ʲ�

	//@EndOf(AbookDemo)
};

