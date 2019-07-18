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

template<typename ElementTy>
struct TVertex
{
	TVertex() {}

	TVertex(const XMFLOAT3 pos)
		: Position(pos) {}

	TVertex(const XMFLOAT3 pos, const XMFLOAT4 color)
		: Position(pos), Color(color) {}

	TVertex(const XMFLOAT3 pos, const XMFLOAT4 color, const XMFLOAT3 normal)
		: Position(pos), Color(color), Normal(normal) {}

	TVertex(const XMFLOAT3 pos, const XMFLOAT4 color, const XMFLOAT3 normal, const XMFLOAT3 tangent)
		: Position(pos), Color(color), Normal(normal), Tangent(tangent) {}

	TVertex(const XMFLOAT3 pos, const XMFLOAT4 color, const XMFLOAT3 normal, const XMFLOAT3 tangent, const XMFLOAT2 tex)
		: Position(pos), Color(color), Normal(normal), Tangent(tangent), Texture(tex) {}

	TVertex(const XMFLOAT3 pos,
		const XMFLOAT4 color,
		const XMFLOAT3 normal,
		const XMFLOAT3 tangent,
		const XMFLOAT2 tex,
		const ElementTy element)
		: Position(pos)
		, Color(color)
		, Normal(normal)
		, Tangent(tangent)
		, Texture(tex), Element(element) {}

	TVertex& operator = (const TVertex& rhs)
	{
		Position = rhs.Position;
		Color = rhs.Color;
		Normal = rhs.Normal;
		Tangent = rhs.Tangent;
		Texture = rhs.Texture;
		Element = rhs.Element;
		return *this;
	}


	XMFLOAT3	Position;	// 位置
	XMFLOAT4	Color;		// 颜色
	XMFLOAT3	Normal;		// 法线
	XMFLOAT3	Tangent;	// 切线U
	XMFLOAT2	Texture;	// 纹理坐标
	ElementTy	Element;	// 顶点要素

	static const UINT OffsetPos = 0;
	static const UINT OffsetClr = OffsetPos + sizeof(Vertex::Position);
	static const UINT OffsetNom = OffsetClr + sizeof(Vertex::Color);
	static const UINT OffsetTan = OffsetNom + sizeof(Vertex::Normal);
	static const UINT OffsetTex = OffsetTan + sizeof(Vertex::Tangent);

};

typedef TVertex<float> FVertex;


ALIBAPI(void) TVertex_test();

