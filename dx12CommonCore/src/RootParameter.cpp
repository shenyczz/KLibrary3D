#include "stdafx.h"
#include "RootParameter.h"

namespace dx12
{

	RootParameter::RootParameter()
	{
		m_RootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
	}

	RootParameter::~RootParameter()
	{
		Clear();
	}

	void RootParameter::Clear()
	{
		if (m_RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			delete[] m_RootParam.DescriptorTable.pDescriptorRanges;
		}

		m_RootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
	}

	// ¸ù³£Á¿
	void RootParameter::InitAsConstants(
		UINT Num32BitValues,
		UINT Register,
		UINT Space,
		D3D12_SHADER_VISIBILITY Visibility)
	{
		D3D12_ROOT_PARAMETER* pRootParam = &m_RootParam;
		pRootParam->ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		pRootParam->ShaderVisibility = Visibility;
		pRootParam->Constants.Num32BitValues = Num32BitValues;
		pRootParam->Constants.ShaderRegister = Register;
		pRootParam->Constants.RegisterSpace = Space;
	}


	// ¸ùÃèÊö·û - CBV 
	void RootParameter::InitAsConstantBufferView(
		UINT Register,
		UINT Space,
		D3D12_SHADER_VISIBILITY Visibility)
	{
		D3D12_ROOT_PARAMETER* pRootParam = &m_RootParam;
		pRootParam->ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		pRootParam->ShaderVisibility = Visibility;
		pRootParam->Descriptor.ShaderRegister = Register;
		pRootParam->Descriptor.RegisterSpace = Space;
	}

	// ¸ùÃèÊö·û - SRV 
	void RootParameter::InitAsShaderResourceView(
		UINT Register,
		UINT Space,
		D3D12_SHADER_VISIBILITY Visibility)
	{
		D3D12_ROOT_PARAMETER* pRootParam = &m_RootParam;
		pRootParam->ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		pRootParam->ShaderVisibility = Visibility;
		pRootParam->Descriptor.ShaderRegister = Register;
		pRootParam->Descriptor.RegisterSpace = Space;
	}

	// ¸ùÃèÊö·û - UAV 
	void RootParameter::InitAsUnorderedAccessView(
		UINT Register,
		UINT Space,
		D3D12_SHADER_VISIBILITY Visibility)
	{
		D3D12_ROOT_PARAMETER* pRootParam = &m_RootParam;
		pRootParam->ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
		pRootParam->ShaderVisibility = Visibility;
		pRootParam->Descriptor.ShaderRegister = Register;
		pRootParam->Descriptor.RegisterSpace = Space;
	}

	// ¸ùÃèÊö·û
	void RootParameter::InitAsDescriptorTable(
		UINT numDescriptorRanges,
		const D3D12_DESCRIPTOR_RANGE* pDescriptorRanges,
		D3D12_SHADER_VISIBILITY Visibility)
	{
		D3D12_ROOT_PARAMETER* pRootParam = &m_RootParam;
		pRootParam->ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		pRootParam->ShaderVisibility = Visibility;
		pRootParam->DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
		pRootParam->DescriptorTable.pDescriptorRanges = pDescriptorRanges;
	}

}//dx12
