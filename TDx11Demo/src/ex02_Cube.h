#pragma once

using namespace dxCommon;


class ex02_Cube : public dx11Sample
{
public:
	ex02_Cube();
	~ex02_Cube();

	struct ConstantBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
	};

private:
	// 重载函数样例
	void BuildAssets() override;
	void BuildShadersAndInputLayout();
	void BuildGeometry0();
	void BuildGeometry1();

	void Resized() override;
	void Update() override;
	void UpdateCB();

	void Draw() override;
	void Shutdown() override { dx11Sample::Shutdown(); }
	void DoAppIdle() override { dx11Sample::DoAppIdle(); }

private:
	MeshData mesh0;

	MeshData* GetMeshData()
	{
		return &mesh0;
	}


	ComPtr<ID3D11InputLayout> m_pVertexLayout;		// 顶点输入布局

	ComPtr<ID3D11Buffer> m_pVertexBuffer;			// 顶点缓冲区
	ComPtr<ID3D11Buffer> m_pIndexBuffer;			// 索引缓冲区
	ComPtr<ID3D11Buffer> m_pConstantBuffer;		    // 常量缓冲区

	ComPtr<ID3D11VertexShader> m_pVertexShader;		// 顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;		// 像素着色器

	ConstantBuffer m_CBuffer;	                    // 用于修改GPU常量缓冲区的变量

	//@EndOf(ex02_Cube)
};

