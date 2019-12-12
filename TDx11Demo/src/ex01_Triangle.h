#pragma once

class ex01_Triangle : public dx11Sample
{
public:
	ex01_Triangle();
	~ex01_Triangle();

private:
	// 重载函数样例
	void BuildAssets() override;
	void BuildShadersAndInputLayout();




	void Resized() override;
	void Update() override;
	void Draw() override;
	void Shutdown() override { dx11Sample::Shutdown(); }
	void DoAppIdle() override { dx11Sample::DoAppIdle(); }


private:
	ComPtr<ID3D11Buffer> m_pVertexBuffer;		// 顶点缓冲区
	ComPtr<ID3D11InputLayout> m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11VertexShader> m_pVertexShader;	// 顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;	// 像素着色器


	//@EndOf(ex01_Triangle)
};

