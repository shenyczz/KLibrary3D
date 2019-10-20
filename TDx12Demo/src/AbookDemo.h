#pragma once

class AbookDemo : public Dx12Book
{
public:
	AbookDemo();
	~AbookDemo();

protected:
	// ����
	void OnResize() override;
	void OnUpdate() override;

	// �����Ϣ����
	void OnMouseDown(WPARAM wParam, int x, int y) override;
	void OnMouseUp(WPARAM wParam, int x, int y) override;
	void OnMouseMove(WPARAM wParam, int x, int y) override;

	// BuildAssets()����
	void BuildRootSignature() override;
	void BuildShadersAndInputLayout() override;
	void BuildMeshData() override;
	void BuildConstantBufferAndView() override;
	void BuildTextureBufferAndView() override;
	void BuildPipelineStateObject() override;

	// OnRender()����
	void BuildCommandList() override;


private:
	// ���ﶨ��˽���ʲ�

	//@EndOf(AbookDemo)
};

