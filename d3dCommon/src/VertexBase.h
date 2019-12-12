#pragma once

namespace dxCommon
{

	struct D3DCOMMON_EXPORTS_CLASS VertexBase
	{
		VertexBase()
			//: DataPointer(nullptr)
		{}

		// {pos}
		VertexBase(const XMFLOAT3 pos)
			: Position(pos)
			//, DataPointer(nullptr)
		{}

		// {pos, clr}
		VertexBase(const XMFLOAT3 pos,
			const XMFLOAT4 clr)
			: Position(pos)
			, Color(clr)
			//, DataPointer(nullptr)
		{}

		// {pos, nom, clr}
		VertexBase(const XMFLOAT3 pos,
			const XMFLOAT3& nom,
			const XMFLOAT4 clr)
			: Position(pos)
			, Normal(nom)
			, Color(clr)
			//, DataPointer(nullptr)
		{}

		// {pos, nom, tng, uv} - 1
		VertexBase(const XMFLOAT3& p,
			const XMFLOAT3& n,
			const XMFLOAT3& t,
			const XMFLOAT2& uv)
			: Position(p)
			, Normal(n)
			, TangentU(t)
			, TextureC(uv)
			, Color(0, 0, 0, 1)
			//, DataPointer(nullptr)
		{}

		// {pos, nom, tng, uv, clr} - 2
		VertexBase(const XMFLOAT3& p,
			const XMFLOAT3& n,
			const XMFLOAT3& t,
			const XMFLOAT2& uv,
			const XMFLOAT4& clr)
			: Position(p)
			, Normal(n)
			, TangentU(t)
			, TextureC(uv)
			, Color(clr)
			//, DataPointer(nullptr)
		{}

		// {pos, nom, tng, uv} - 2
		VertexBase(float px, float py, float pz,	// pos
			float nx, float ny, float nz,			// nom
			float tx, float ty, float tz,			// tng
			float u, float v)						// uv
			: Position(px, py, pz)
			, Normal(nx, ny, nz)
			, TangentU(tx, ty, tz)
			, TextureC(u, v)
			, Color(0, 0, 0, 1)
			//, DataPointer(nullptr)
		{}


		// {pos, nom, tng, uv, clr} - 2
		VertexBase(float px, float py, float pz,	// pos
			float nx, float ny, float nz,			// nom
			float tx, float ty, float tz,			// tng
			float u, float v,						// uv
			float r, float g, float b, float a)		// clr
			: Position(px, py, pz)
			, Normal(nx, ny, nz)
			, TangentU(tx, ty, tz)
			, TextureC(u, v)
			, Color(r, g, b, a)
			//, DataPointer(nullptr)
		{}



		VertexBase& operator = (const VertexBase& rhs)
		{
			Position = rhs.Position;
			Normal = rhs.Normal;
			TangentU = rhs.TangentU;
			TextureC = rhs.TextureC;
			Color = rhs.Color;
			//DataPointer = rhs.DataPointer;
			return *this;
		}

		//void SetData(const void* pData)
		//{
		//	DataPointer = (LPVOID)(pData);
		//}

		//void* GetData() const
		//{
		//	return DataPointer;
		//}

		XMFLOAT3	Position;		// 位置				- 12
		XMFLOAT3	Normal;			// 法线				- 12
		XMFLOAT3	TangentU;		// 切线U			- 12
		XMFLOAT2	TextureC;		// 纹理坐标			- 8
		XMFLOAT4	Color;			// 颜色				- 16
		//LPVOID		DataPointer;	// 顶点数据指针		- 4

		static const UINT OffsetPos = 0;											// 0
		static const UINT OffsetNom = OffsetPos + sizeof(VertexBase::Position);		// 12
		static const UINT OffsetTan = OffsetNom + sizeof(VertexBase::Normal);		// 24
		static const UINT OffsetTex = OffsetTan + sizeof(VertexBase::TangentU);		// 36
		static const UINT OffsetClr = OffsetTex + sizeof(VertexBase::TextureC);		// 44

	};

	ALIBAPI(void) alib_test_VertexBase();

	typedef dxCommon::VertexBase Vertex;

}
