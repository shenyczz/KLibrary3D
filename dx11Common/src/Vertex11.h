#pragma once

namespace dx11
{
	struct DX11LIBCOMMON_EXPORTS_CLASS Vertex11 : public dxCommon::VertexBase
	{
		/*
		Vertex11()
			: dxCommon::VertexBase() {}

		// {pos}
		Vertex11(const XMFLOAT3 pos)
			: dxCommon::VertexBase(pos) {}

		// {pos, clr}
		Vertex11(const XMFLOAT3 pos,
			const XMFLOAT4 clr)
			: dxCommon::VertexBase(pos, clr) {}

		// {pos, nom, clr}
		Vertex11(const XMFLOAT3 pos,
			const XMFLOAT3& nom,
			const XMFLOAT4 clr)
			: dxCommon::VertexBase(pos, nom, clr) {}

		// {pos, nom, tng, uv} - 1
		Vertex11(const XMFLOAT3& p,
			const XMFLOAT3& n,
			const XMFLOAT3& t,
			const XMFLOAT2& uv)
			: dxCommon::VertexBase(p, n, t, uv) {}

		// {pos, nom, tng, uv, clr} - 1
		Vertex11(const XMFLOAT3& p,
			const XMFLOAT3& n,
			const XMFLOAT3& t,
			const XMFLOAT2& uv,
			const XMFLOAT4& clr)
			: dxCommon::VertexBase(p, n, t, uv, clr) {}

		// {pos, nom, tng, uv} - 2
		Vertex11(float px, float py, float pz,	// pos
			float nx, float ny, float nz,		// nom
			float tx, float ty, float tz,		// tng
			float u, float v)					// uv
			: dxCommon::VertexBase(
				px, py, pz,		// pos
				nx, ny, nz,		// nom
				tx, ty, tz,		// tng
				u, v) {}		// uv


		// {pos, nom, tng, uv, clr} - 2
		Vertex11(float px, float py, float pz,	// pos
			float nx, float ny, float nz,		// nom
			float tx, float ty, float tz,		// tng
			float u, float v,					// uv
			float r, float g, float b, float a)	// clr
			: dxCommon::VertexBase(
				px, py, pz,		// pos
				nx, ny, nz,		// nom
				tx, ty, tz,		// tng
				u, v,			// uv
				r, g, b, a) {}	// clr

		*/

		static const std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayout;

		static const D3D11_INPUT_ELEMENT_DESC InputLayout_bak[5];
	};

	ALIBAPI(void) alib_test_Vertex11();

}

