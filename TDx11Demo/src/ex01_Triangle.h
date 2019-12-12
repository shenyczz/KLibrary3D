#pragma once

class ex01_Triangle : public dx11Sample
{
public:
	ex01_Triangle();
	~ex01_Triangle();

private:
	// ���غ�������
	void BuildAssets() override;
	void BuildShadersAndInputLayout();




	void Resized() override;
	void Update() override;
	void Draw() override;
	void Shutdown() override { dx11Sample::Shutdown(); }
	void DoAppIdle() override { dx11Sample::DoAppIdle(); }


private:
	ComPtr<ID3D11Buffer> m_pVertexBuffer;		// ���㻺����
	ComPtr<ID3D11InputLayout> m_pVertexLayout;	// �������벼��
	ComPtr<ID3D11VertexShader> m_pVertexShader;	// ������ɫ��
	ComPtr<ID3D11PixelShader> m_pPixelShader;	// ������ɫ��


	//@EndOf(ex01_Triangle)
};

