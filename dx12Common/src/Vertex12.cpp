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
		"POSITION",									// SemanticName			- 依据 HLSL
		0,											// SemanticIndex		- 依据 HLSL
		DXGI_FORMAT_R32G32B32_FLOAT,				// Format				- 依据 Vertex(32位浮点数)
		0,											// InputSlot			- 输入槽
		Vertex::OffsetPos,							// AlignedByteOffset	- 对齐字节偏移
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	// InputSlotClass		- 输入槽类型
		0 },										// InstanceDataStepRate	- 实例数据步长

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
