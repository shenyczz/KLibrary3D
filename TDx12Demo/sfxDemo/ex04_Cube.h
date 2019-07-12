#pragma once


class Cube : public Dx12Sample
{
public:
	Cube();
	~Cube();

private:
	struct Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	struct ObjectConstants
	{
		XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	};

protected:
	void InitializeGraphics() override;
	void InitializeAssets() override;

	void WindowResized() override;
	void UpdateParameter() override;
	void PopulateCommandList() override;

	void CreateRootSignature() override;

	void BuildByteCode() override;
	void BuildInputElementDescrips() override;
	void BuildPipelineStateObject() override;
	void BuildVertexBufferAndView() override;
	void BuildIndexBufferAndView() override;
	void BuildConstantBufferAndView() override;

private:
	//void BuildConstantBuffers();
	//void BuildShadersAndInputLayout() {}
	//void BuildBoxGeometry() {}








private:
	std::unique_ptr<UploadBuffer<ObjectConstants>> m_ObjectCB = nullptr;
	std::unique_ptr<MeshGeometry> m_BoxGeo = nullptr;







};

