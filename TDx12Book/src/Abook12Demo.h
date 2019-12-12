/******************************************************************************
*
* Usage:
*
* [01]创建类 Ademo, 基类为 Dx12Sample
* [02]修改构造函数为参数构造函数，因为基类 Dx12Sample 默认构造函数被删除
* [03]参数构造函数要运行基类的参数构造函数
* [04]重载以下函数
*
*	void BuildAssets() override;
*	void Resized() override;
*	void Update() override;
*	void Draw() override;
*
*	void DoAppIdle() override;	// 必要时重载
*
* [05]必要时重载以下函数
*	// 启动
*	void OnInit() override;
*	void OnResize() override;
*	void OnUpdate() override;
*	void OnRender() override;
*
*	// 鼠标消息处理
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
	// 在此定义私有资产
	bool m_IsWireframe;

	ComPtr<ID3D12RootSignature> m_RootSignature;

	std::unordered_map<_astring, std::unique_ptr<MeshGeometry>> m_Geometries;

	//@EndOf(AbookDemo)
};

