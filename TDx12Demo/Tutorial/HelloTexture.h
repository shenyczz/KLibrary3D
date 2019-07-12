#pragma once

class HelloTexture : public Dx12Sample
{
public:
	HelloTexture() {}
	~HelloTexture() {}

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

private:
	static const UINT TextureWidth = 256;
	static const UINT TextureHeight = 256;
	static const UINT TexturePixelSize = 4;	// The number of bytes used to represent a pixel in the texture.

protected:
	virtual void InitializeAssets() override;
	virtual void InitializeAssets_ok();
	virtual void InitializeAssets2();
	virtual void PopulateCommandList() override;

	void CreateRootSignature() override;

	void BuildByteCode() override;
	void BuildInputElementDescrips() override;
	void BuildPipelineStateObject() override;
	void BuildVertexBufferAndView() override;
	//void BuildIndexBufferAndView() override;
	void BuildTextureBufferAndView() override;
	//void BuildConstantBufferAndView() override;

private:
	std::vector<UINT8> GenerateTextureData();


};

