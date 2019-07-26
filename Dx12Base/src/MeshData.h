/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: 申永辰.郑州 (shenyczz@163.com)
*  WebSite: http://github.com/shenyczz/KLibrary
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
******************************************************************************/
#pragma once

#include "Vertex.h"

namespace DX12
{

	struct MeshData
	{
	public:
		MeshData() {}
		MeshData(const MeshData& rhs)
		{
			Vertices = rhs.Vertices;
			Indices32 = rhs.Indices32;
		}

	public:
		void Clear()
		{
			Vertices.clear();
			Indices32.clear();
		}

		void Offset(float dx, float dy, float dz)
		{
			int n = Vertices.size();
			for (int i = 0; i < n; i++)
			{
				Vertices[i].Position.x += dx;
				Vertices[i].Position.y += dy;
				Vertices[i].Position.z += dz;

			}
		}
		void OffsetX(float dx) { Offset(dx, 0, 0); }
		void OffsetY(float dy) { Offset(0, dy, 0); }
		void OffsetZ(float dz) { Offset(0, 0, dz); }


		const UINT IndexCount() const { return Indices32.size(); }
		const UINT VertexCount() const { return Vertices.size(); }

		// 创建 VBView 用
		const void* pvbInitData() const { return this->Vertices.data(); }
		const UINT vbSizeInBytes() const { return sizeof(Vertex) * Vertices.size(); }
		const UINT vbStrideInBytes() const { return sizeof(Vertex); }

		// 创建 IBView 用
		const void* pibInitData() const { return this->Indices32.data(); }
		const UINT ibSizeInBytes() const { return Indices32.size() * sizeof(std::uint32_t); }

		std::vector<uint16>& GetIndices16()
		{
			if (m_Indices16.empty())
			{
				m_Indices16.resize(Indices32.size());
				for (size_t i = 0; i < Indices32.size(); ++i)
				{
					m_Indices16[i] = static_cast<uint16>(Indices32[i]);
				}
			}

			return m_Indices16;
		}


	public:
		std::vector<Vertex>	Vertices;
		std::vector<uint32> Indices32;

		static const DXGI_FORMAT IBVFormat = DXGI_FORMAT_R32_UINT;

	private:
		std::vector<uint16> m_Indices16;



		//@EndOf(MeshData)
	};
}