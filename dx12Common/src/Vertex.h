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

namespace dx12
{

	struct Vertex
	{
		Vertex()
			: DataPointer(nullptr) {}

		// {pos}
		Vertex(const XMFLOAT3 pos)
			: Position(pos)
			, DataPointer(nullptr) {}

		// {pos, clr}
		Vertex(const XMFLOAT3 pos,
			const XMFLOAT4 clr)
			: Position(pos)
			, Color(clr)
			, DataPointer(nullptr) {}

		// {pos, nom, clr}
		Vertex(const XMFLOAT3 pos,
			const XMFLOAT3& nom,
			const XMFLOAT4 clr)
			: Position(pos)
			, Normal(nom)
			, Color(clr)
			, DataPointer(nullptr) {}

		// {pos, nom, tng, uv}
		Vertex(const XMFLOAT3& p,
			const XMFLOAT3& n,
			const XMFLOAT3& t,
			const XMFLOAT2& uv)
			: Position(p)
			, Normal(n)
			, TangentU(t)
			, TextureC(uv)
			, Color(0, 0, 0, 1)
			, DataPointer(nullptr) {}

		// {pos, nom, tng, uv, clr}
		Vertex(const XMFLOAT3& p,
			const XMFLOAT3& n,
			const XMFLOAT3& t,
			const XMFLOAT2& uv,
			const XMFLOAT4& clr)
			: Position(p)
			, Normal(n)
			, TangentU(t)
			, TextureC(uv)
			, Color(clr)
			, DataPointer(nullptr) {}

		Vertex(float px, float py, float pz,	// pos
			float nx, float ny, float nz,		// nom
			float tx, float ty, float tz,		// tng
			float u, float v)					// uv
			: Position(px, py, pz)
			, Normal(nx, ny, nz)
			, TangentU(tx, ty, tz)
			, TextureC(u, v)
			, Color(0, 0, 0, 1)
			, DataPointer(nullptr) {}


		Vertex(float px, float py, float pz,	// pos
			float nx, float ny, float nz,		// nom
			float tx, float ty, float tz,		// tng
			float u, float v,					// uv
			float r, float g, float b, float a)	// clr
			: Position(px, py, pz)
			, Normal(nx, ny, nz)
			, TangentU(tx, ty, tz)
			, TextureC(u, v)
			, Color(r, g, b, a)
			, DataPointer(nullptr) {}



		Vertex& operator = (const Vertex& rhs)
		{
			Position = rhs.Position;
			Normal = rhs.Normal;
			TangentU = rhs.TangentU;
			TextureC = rhs.TextureC;
			Color = rhs.Color;
			DataPointer = rhs.DataPointer;
			return *this;
		}

		void SetData(const void* pData)
		{
			DataPointer = (LPVOID)(pData);
		}

		void* GetData() const
		{
			return DataPointer;
		}

		XMFLOAT3	Position;		// 位置
		XMFLOAT3	Normal;			// 法线
		XMFLOAT3	TangentU;		// 切线U
		XMFLOAT2	TextureC;		// 纹理坐标
		XMFLOAT4	Color;			// 颜色
		LPVOID		DataPointer;	// 顶点数据指针

		static const UINT OffsetPos = 0;
		static const UINT OffsetNom = OffsetPos + sizeof(Vertex::Position);
		static const UINT OffsetTan = OffsetNom + sizeof(Vertex::Normal);
		static const UINT OffsetTex = OffsetTan + sizeof(Vertex::TangentU);
		static const UINT OffsetClr = OffsetTex + sizeof(Vertex::TextureC);

	};

	typedef std::vector<Vertex> VertexCollection;
	typedef std::vector<uint32> IndexCollection;

	ALIBAPI(void) TVertex_test();

}//DX12Library
