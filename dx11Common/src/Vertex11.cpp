#include "stdafx.h"
#include "Vertex11.h"

namespace dx11
{
	using namespace dxCommon;

	ALIBAPI(void) alib_test_Vertex11()
	{
		//const D3D11_INPUT_ELEMENT_DESC* p = Vertex::InputLayout1.data();
	} 

	//static
	const std::vector<D3D11_INPUT_ELEMENT_DESC> Vertex11::InputLayout =
	{
		D3D11_INPUT_ELEMENT_DESC{
		"POSITION",						// SemanticName			- ���� HLSL
		0,								// SemanticIndex		- ���� HLSL
		DXGI_FORMAT_R32G32B32_FLOAT,	// Format				- ���� Vertex(32λ������)
		0,								// InputSlot			- �����
		Vertex::OffsetPos,				// AlignedByteOffset	- �����ֽ�ƫ��
		D3D11_INPUT_PER_VERTEX_DATA,	// InputSlotClass		- ���������
		0 },							// InstanceDataStepRate	- ʵ�����ݲ���

		D3D11_INPUT_ELEMENT_DESC{
		"NORMAL", 0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0, Vertex::OffsetNom,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },

		D3D11_INPUT_ELEMENT_DESC{
		"TANGENT", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0, Vertex::OffsetTan,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },

		D3D11_INPUT_ELEMENT_DESC{
		"TEXCOORD", 0,
		DXGI_FORMAT_R32G32_FLOAT,
		0, Vertex::OffsetTex,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },

		D3D11_INPUT_ELEMENT_DESC{
		"COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0, Vertex::OffsetClr,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};


	//static
	const D3D11_INPUT_ELEMENT_DESC Vertex11::InputLayout_bak[5] =
	{
		D3D11_INPUT_ELEMENT_DESC{
		"POSITION",						// SemanticName			- ���� HLSL
		0,								// SemanticIndex		- ���� HLSL
		DXGI_FORMAT_R32G32B32_FLOAT,	// Format				- ���� Vertex(32λ������)
		0,								// InputSlot			- �����
		Vertex::OffsetPos,				// AlignedByteOffset	- �����ֽ�ƫ��
		D3D11_INPUT_PER_VERTEX_DATA,	// InputSlotClass		- ���������
		0 },							// InstanceDataStepRate	- ʵ�����ݲ���

		D3D11_INPUT_ELEMENT_DESC{
		"NORMAL", 0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0, Vertex::OffsetNom,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },

		D3D11_INPUT_ELEMENT_DESC{
		"TANGENT", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0, Vertex::OffsetTan,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },

		D3D11_INPUT_ELEMENT_DESC{
		"TEXCOORD", 0,
		DXGI_FORMAT_R32G32_FLOAT,
		0, Vertex::OffsetTex,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },

		D3D11_INPUT_ELEMENT_DESC{
		"COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0, Vertex::OffsetClr,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

}
