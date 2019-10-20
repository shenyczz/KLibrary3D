#pragma once

class Abook2Demo : public Dx12Book2
{
public:
	Abook2Demo();
	~Abook2Demo();

protected:
	// 启动
	void OnResize() override;
	void OnUpdate() override;
	void OnRender() override;

	// 鼠标消息处理
	void OnMouseDown(WPARAM wParam, int x, int y) override;
	void OnMouseUp(WPARAM wParam, int x, int y) override;
	void OnMouseMove(WPARAM wParam, int x, int y) override;

	// BuildAssets()调用
	void BuildRootSignature() override;
	void BuildShadersAndInputLayout() override;
	void BuildPipelineStateObject() override;
	void BuildMeshData() override;
	void BuildOthers() override;

	// OnRender()调用
	void BuildCommandList() override;


private:
	// 这里定义私有资产

	//@EndOf(AbookDemo)
};

