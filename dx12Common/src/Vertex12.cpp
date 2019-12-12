#include "stdafx.h"
#include "Vertex12.h"

namespace dx12
{
	using namespace dxCommon;

	ALIBAPI(void) alib_test_Vertex12()
	{

	}

	//static
	const std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout =
	{
		D3D12_INPUT_ELEMENT_DESC{
		"POSITION",									// SemanticName			- ���� HLSL
		0,											// SemanticIndex		- ���� HLSL
		DXGI_FORMAT_R32G32B32_FLOAT,				// Format				- ���� Vertex(32λ������)
		0,											// InputSlot			- �����
		Vertex::OffsetPos,							// AlignedByteOffset	- �����ֽ�ƫ��
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	// InputSlotClass		- ���������
		0 },										// InstanceDataStepRate	- ʵ�����ݲ���

		D3D12_INPUT_ELEMENT_DESC{
		"NORMAL", 0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0, Vertex::OffsetNom,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		D3D12_INPUT_ELEMENT_DESC{
		"TANGENT", 0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0, Vertex::OffsetTan,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		D3D12_INPUT_ELEMENT_DESC{
		"TEXCOORD", 0,
		DXGI_FORMAT_R32G32_FLOAT,
		0, Vertex::OffsetTex,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		D3D12_INPUT_ELEMENT_DESC{
		"COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0, Vertex::OffsetClr,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};


}
