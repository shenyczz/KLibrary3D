#include "stdafx.h"
#include "Abook2Demo.h"


Abook2Demo::Abook2Demo()
{
}

Abook2Demo::~Abook2Demo()
{
}


void Abook2Demo::OnResize() { Dx12Book2::OnResize(); }			// Dx12Book2::OnResize()					- 必须调用
void Abook2Demo::OnUpdate() {}									// Dx12Book2::OnUpdate()					- no body
void Abook2Demo::OnRender() {}									// Dx12Book2::OnRender()					- 参考代码
void Abook2Demo::OnMouseDown(WPARAM wParam, int x, int y) {}	// Dx12Book2::OnMouseDown(wParam, x, y)		- 默认代码
void Abook2Demo::OnMouseUp(WPARAM wParam, int x, int y) {}		// Dx12Book2::OnMouseUp(wParam, x, y)		- 默认代码
void Abook2Demo::OnMouseMove(WPARAM wParam, int x, int y) {}	// Dx12Book2::OnMouseMove(wParam, x, y)		- 默认代码
void Abook2Demo::BuildRootSignature() {}						// Dx12Book2::BuildRootSignature()			- no body
void Abook2Demo::BuildShadersAndInputLayout() {}				// Dx12Book2::BuildShadersAndInputLayout()	- no body
void Abook2Demo::BuildPipelineStateObject() {}					// Dx12Book2::BuildPipelineStateObject()	- no body
void Abook2Demo::BuildMeshData() {}								// Dx12Book2::BuildMeshData()				- no body
void Abook2Demo::BuildOthers() {}								// Dx12Book2::BuildConstantBufferAndView()	- no body
void Abook2Demo::BuildCommandList() {}							// Dx12Book2::BuildCommandList()			- 参考代码


