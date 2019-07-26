#pragma once

class AbookDemo : public Dx12Book
{
public:
	AbookDemo();
	~AbookDemo();

protected:
	void OnResize() override;
	void OnUpdate() override;

	// 鼠标消息处理
	void OnMouseDown(WPARAM wParam, int x, int y) override;
	void OnMouseUp(WPARAM wParam, int x, int y) override;
	void OnMouseMove(WPARAM wParam, int x, int y) override;

	// BuildAssets()调用
	void BuildRootSignature() override;
	void BuildShadersAndInputLayout() override;
	void BuildPipelineStateObject() override;
	void BuildMeshData() override;
	void BuildConstantBufferAndView() override;
	void BuildTextureBufferAndView() override;

	// OnRender()调用
	void ReccorCommand() override;


private:
	//std::unordered_map<PsoStyle, ComPtr<ID3D12PipelineState>> m_psoMap;

};

