#pragma once

namespace dx12
{

	class DX12LIBCOMMONCORE_CLASS DescriptorRange
	{
	public:
		DescriptorRange();
		~DescriptorRange();

	public:
		const D3D12_DESCRIPTOR_RANGE& operator() (void) const { return m_Range; }


	private:
		D3D12_DESCRIPTOR_RANGE m_Range;
	};

}
