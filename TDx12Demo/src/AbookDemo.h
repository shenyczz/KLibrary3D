#pragma once

class AbookDemo : public Dx12Book
{
public:
	AbookDemo();
	~AbookDemo();

protected:
	// 启动
	void OnResize() override;
	void OnUpdate() override;

	// 鼠标消息处理
	void OnMouseDown(WPARAM wParam, int x, int y) override;
	void OnMouseUp(WPARAM wParam, int x, int y) override;
	void OnMouseMove(WPARAM wParam, int x, int y) override;

	// BuildAssets()调用
	void BuildRootSignature() override;
	void BuildShadersAndInputLayout() override;
	void BuildMeshData() override;
	void BuildConstantBufferAndView() override;
	void BuildTextureBufferAndView() override;
	void BuildPipelineStateObject() override;

	// OnRender()调用
	void BuildCommandList() override;


private:
	// 这里定义私有资产

	//@EndOf(AbookDemo)
};

