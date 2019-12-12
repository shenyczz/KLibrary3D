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
	// ���غ�������
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


	ComPtr<ID3D11InputLayout> m_pVertexLayout;		// �������벼��

	ComPtr<ID3D11Buffer> m_pVertexBuffer;			// ���㻺����
	ComPtr<ID3D11Buffer> m_pIndexBuffer;			// ����������
	ComPtr<ID3D11Buffer> m_pConstantBuffer;		    // ����������

	ComPtr<ID3D11VertexShader> m_pVertexShader;		// ������ɫ��
	ComPtr<ID3D11PixelShader> m_pPixelShader;		// ������ɫ��

	ConstantBuffer m_CBuffer;	                    // �����޸�GPU�����������ı���

	//@EndOf(ex02_Cube)
};

