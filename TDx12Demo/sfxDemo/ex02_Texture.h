#pragma once

class Texture : public Dx12Sample
{
public:
	Texture();
	~Texture();

private:
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
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
	void BuildTextureBufferAndView() override;
	void BuildConstantBufferAndView() override;



private:
	static const UINT TextureWidth = 256;
	static const UINT TextureHeight = 256;
	static const UINT TexturePixelSize = 4;	// The number of bytes used to represent a pixel in the texture.


private:
	std::vector<UINT8> GenerateTextureData();


};

