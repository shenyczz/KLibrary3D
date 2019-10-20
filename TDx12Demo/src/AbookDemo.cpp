#include "stdafx.h"
#include "AbookDemo.h"


AbookDemo::AbookDemo()
{
}

AbookDemo::~AbookDemo()
{
}



void AbookDemo::OnResize() { Dx12Book::OnResize(); }			// Dx12Book::OnResize()						- 必须调用
void AbookDemo::OnUpdate() {}									// Dx12Book::OnUpdate()						- no body
void AbookDemo::OnMouseDown(WPARAM wParam, int x, int y) {}		// Dx12Book::OnMouseDown(wParam, x, y)		- 默认代码
void AbookDemo::OnMouseUp(WPARAM wParam, int x, int y) {}		// Dx12Book::OnMouseUp(wParam, x, y)		- 默认代码
void AbookDemo::OnMouseMove(WPARAM wParam, int x, int y) {}		// Dx12Book::OnMouseMove(wParam, x, y)		- 默认代码
void AbookDemo::BuildRootSignature() {}							// Dx12Book::BuildRootSignature()			- no body
void AbookDemo::BuildShadersAndInputLayout() {}					// Dx12Book::BuildShadersAndInputLayout()	- no body
void AbookDemo::BuildMeshData() {}								// Dx12Book::BuildMeshData()				- no body
void AbookDemo::BuildConstantBufferAndView() {}					// Dx12Book::BuildConstantBufferAndView()	- no body
void AbookDemo::BuildTextureBufferAndView() {}					// Dx12Book::BuildTextureBufferAndView()	- no body
void AbookDemo::BuildPipelineStateObject() {}					// Dx12Book::BuildPipelineStateObject()		- no body
void AbookDemo::BuildCommandList() {}							// Dx12Book::BuildCommandList()				- 参考代码
