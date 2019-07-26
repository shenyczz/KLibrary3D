#pragma once

#include "FrameResource.h"

class Shapes : public Dx12Book
{
public:
	Shapes();
	~Shapes();

protected:
	void OnResize() override;
	void OnUpdate() override;


protected:
	void BuildRootSignature() override;

	void BuildShadersAndInputLayout() override;
	void BuildPipelineStateObject() override;

	void BuildMeshData() override;

	void BuildConstantBufferAndView() override;

	void ReccorCommand() override;


private:
	void UpdateCamera();

private:
	XMFLOAT3 m_EyePos = { 0.0f, 0.0f, 0.0f };

	//@EndOf(Shapes)
};

