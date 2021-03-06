#include "stdafx.h"
#include "MeshFactory.h"

namespace dxCommon
{
	//static
	void MeshFactory::CreateTriangle(const float side, MeshData* pMeshData)
	{
		pMeshData->Clear();

		float slen = side;

		/*

		// 左手，原点在立方体中心
		//	       v1
		//	       *
		//	      / \
		//	     /   \
		//	    /     \
		//	   /       \
		//	  /         \
		//	 *-----------*
		//	v0           v2
		//

		Vertex triangleVertices[] =
		{
			{ { +0.00f, +0.25f}, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { +0.25f, -0.25f}, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f}, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		*/

		std::vector<XMFLOAT3> pnts =
		{
			XMFLOAT3(+0.00f, +0.25f, 0.0f),
			XMFLOAT3(+0.25f, -0.25f, 0.0f),
			XMFLOAT3(-0.25f, -0.25f, 0.0f),
		};

		std::vector<XMFLOAT3> noms =
		{
			XMFLOAT3(+0.0f, +0.0f, +0.0f),
			XMFLOAT3(+0.0f, +0.0f, +0.0f),
			XMFLOAT3(+0.0f, +0.0f, +0.0f),
		};

		std::vector<XMFLOAT3> tans =
		{
			XMFLOAT3(+0.0f, +0.0f, +0.0f),
			XMFLOAT3(+0.0f, +0.0f, +0.0f),
			XMFLOAT3(+0.0f, +0.0f, +0.0f),
		};

		std::vector<XMFLOAT2> texs =
		{
			XMFLOAT2(0.5f, 0.0f),
			XMFLOAT2(1.0f, 1.0f),
			XMFLOAT2(0.0f, 1.0f),
		};

		std::vector<XMFLOAT4> clrs =
		{
			XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
			XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
			XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		};

		// 顶点数据
		for (int i = 0; i < 3; i++)
		{
			XMFLOAT3 pnt = { pnts[i].x, pnts[i].y, pnts[i].z };
			pMeshData->Vertices.push_back(Vertex(pnts[i], noms[i], tans[i], texs[i], clrs[i]));
		}

		// 索引数据
		pMeshData->Indices32 = { 0, 1, 2, };

		return;
	}

	//static
	void MeshFactory::CreateQuad(float x, float y, float w, float h, float depth, MeshData* pMeshData)
	{
		pMeshData->Clear();

		pMeshData->Vertices.resize(4);
		pMeshData->Indices32.resize(6);

		// TODO：NDC space.?
		// Position coordinates specified in NDC space.
		pMeshData->Vertices[0] = Vertex(
			x, y - h, depth,	// p
			0.0f, 0.0f, -1.0f,	// n
			1.0f, 0.0f, 0.0f,	// t
			0.0f, 1.0f);		// uv

		pMeshData->Vertices[1] = Vertex(
			x, y, depth,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f);

		pMeshData->Vertices[2] = Vertex(
			x + w, y, depth,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f);

		pMeshData->Vertices[3] = Vertex(
			x + w, y - h, depth,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 1.0f);

		pMeshData->Indices32 =
		{
			0, 1, 2,
			2, 3, 0,
		};

		return;
	}

	// width	- 宽度 x
	// depth	- 深度 z
	// m		- 行数（对应深度 z）
	// n		- 列数（对应宽度 x）
	void MeshFactory::CreateGrid(float width, float depth, uint32 m, uint32 n, MeshData* pMeshData)
	{
		pMeshData->Clear();

		uint32 vertexCount = m * n;
		uint32 faceCount = (m - 1)*(n - 1) * 2;

		//
		// Create the vertices.
		//

		float halfWidth = 0.5f * width;
		float halfDepth = 0.5f * depth;

		float dx = width / (n - 1);
		float dz = depth / (m - 1);

		float du = 1.0f / (n - 1);
		float dv = 1.0f / (m - 1);

		pMeshData->Vertices.resize(vertexCount);
		for (uint32 i = 0; i < m; ++i)
		{
			// z: 0.5 => -0.5
			float z = halfDepth - i * dz;
			for (uint32 j = 0; j < n; ++j)
			{
				// x: -0.5 => 0.5
				float x = -halfWidth + j * dx;

				pMeshData->Vertices[i*n + j].Position = XMFLOAT3(x, 0.0f, z);
				pMeshData->Vertices[i*n + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				pMeshData->Vertices[i*n + j].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);

				// Stretch texture over grid.
				pMeshData->Vertices[i*n + j].TextureC.x = j * du;
				pMeshData->Vertices[i*n + j].TextureC.y = i * dv;
			}
		}

		//
		// Create the indices. 3 indices per face
		//
		pMeshData->Indices32.resize(faceCount * 3);

		// Iterate over each quad and compute indices.
		uint32 k = 0;
		for (uint32 i = 0; i < m - 1; ++i)
		{
			for (uint32 j = 0; j < n - 1; ++j)
			{
				pMeshData->Indices32[k + 0] = i * n + j;
				pMeshData->Indices32[k + 1] = i * n + j + 1;
				pMeshData->Indices32[k + 2] = (i + 1)*n + j;

				pMeshData->Indices32[k + 3] = (i + 1)*n + j;
				pMeshData->Indices32[k + 4] = i * n + j + 1;
				pMeshData->Indices32[k + 5] = (i + 1)*n + j + 1;

				k += 6; // next quad
			}
		}

		return;
	}


	//static
	void MeshFactory::CreateBox(MeshData* pMeshData)
	{
		pMeshData->Clear();

		// 设置立方体顶点
		//    5________ 6
		//    /|      /|
		//   /_|_____/ |
		//  1|4|_ _ 2|_|7
		//   | /     | /
		//   |/______|/
		//  0       3
		std::vector<Vertex> vertices =
		{
			// pos, clr
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }
		};

		// 索引数组
		//
		std::vector<WORD> indices = {
			// 正面
			0, 1, 2,
			2, 3, 0,
			// 左面
			4, 5, 1,
			1, 0, 4,
			// 顶面
			1, 5, 6,
			6, 2, 1,
			// 背面
			7, 6, 5,
			5, 4, 7,
			// 右面
			3, 2, 6,
			6, 7, 3,
			// 底面
			4, 0, 3,
			3, 7, 4
		};

		*pMeshData = MeshData(vertices, indices);

		return;
	}



	// 基本立方体盒子
	// width			- 宽度 x
	// height			- 高度 y
	// depth			- 深度 z
	void MeshFactory::CreateBox(float width, float height, float depth, MeshData* pMeshData)
	{
		pMeshData->Clear();

		// A box has six faces, each one pointing in a different direction.
		const int FaceCount = 6;

		static const XMVECTORF32 faceNormals[FaceCount] =
		{
			{ { {  0,  0,  1, 0 } } },
			{ { {  0,  0, -1, 0 } } },
			{ { {  1,  0,  0, 0 } } },
			{ { { -1,  0,  0, 0 } } },
			{ { {  0,  1,  0, 0 } } },
			{ { {  0, -1,  0, 0 } } },
		};

		static const XMVECTORF32 textureCoordinates[4] =
		{
			{ { { 1, 0, 0, 0 } } },
			{ { { 1, 1, 0, 0 } } },
			{ { { 0, 1, 0, 0 } } },
			{ { { 0, 0, 0, 0 } } },
		};

		XMVECTOR tsize = XMLoadFloat3(&XMFLOAT3{ width,  height,  depth });
		tsize = XMVectorDivide(tsize, g_XMTwo);

		// Create each face in turn.
		for (int i = 0; i < FaceCount; i++)
		{
			XMVECTOR normal = faceNormals[i];

			// Get two vectors perpendicular both to the face normal and to each other.
			XMVECTOR basis = (i >= 4) ? g_XMIdentityR2 : g_XMIdentityR1;

			XMVECTOR side1 = XMVector3Cross(normal, basis);
			XMVECTOR side2 = XMVector3Cross(normal, side1);

			// Six indices (two triangles) per face.
			size_t vbase = pMeshData->VertexCount();
			pMeshData->index_push_back(vbase + 0);
			pMeshData->index_push_back(vbase + 1);
			pMeshData->index_push_back(vbase + 2);

			pMeshData->index_push_back(vbase + 0);
			pMeshData->index_push_back(vbase + 2);
			pMeshData->index_push_back(vbase + 3);

			// Four vertices per face.
			Vertex vert;
			XMVECTOR position = (normal - side1 - side2) * tsize;
			XMStoreFloat3(&vert.Position, position);
			XMStoreFloat3(&vert.Normal, normal);
			XMStoreFloat2(&vert.TextureC, textureCoordinates[0]);
			XMStoreFloat4(&vert.Color, Colors::Red);
			pMeshData->Vertices.push_back(vert);

			position = (normal - side1 + side2) * tsize;
			XMStoreFloat3(&vert.Position, position);
			XMStoreFloat3(&vert.Normal, normal);
			XMStoreFloat2(&vert.TextureC, textureCoordinates[1]);
			XMStoreFloat4(&vert.Color, Colors::Green);
			pMeshData->Vertices.push_back(vert);

			position = (normal + side1 + side2) * tsize;
			XMStoreFloat3(&vert.Position, position);
			XMStoreFloat3(&vert.Normal, normal);
			XMStoreFloat2(&vert.TextureC, textureCoordinates[2]);
			XMStoreFloat4(&vert.Color, Colors::Cyan);
			pMeshData->Vertices.push_back(vert);

			position = (normal + side1 - side2) * tsize;
			XMStoreFloat3(&vert.Position, position);
			XMStoreFloat3(&vert.Normal, normal);
			XMStoreFloat2(&vert.TextureC, textureCoordinates[3]);
			XMStoreFloat4(&vert.Color, Colors::Yellow);
			pMeshData->Vertices.push_back(vert);
		}

		pMeshData->ReverseWinding();

		return;
	}

	//static
	// width			- 宽度 x
	// height			- 高度 y
	// depth			- 深度 z
	// numSubdivisions	- 细分次数
	void MeshFactory::CreateBox(float width, float height, float depth, uint32 numSubdivisions, MeshData* pMeshData)
	{
		pMeshData->Clear();

		// Create the vertices.
		Vertex v[24];

		float w2 = 0.5f*width;
		float h2 = 0.5f*height;
		float d2 = 0.5f*depth;

		// Fill in the front face vertex data.
		v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the back face vertex data.
		v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the top face vertex data.
		v[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the bottom face vertex data.
		v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the left face vertex data.
		v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// Fill in the right face vertex data.
		v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

		pMeshData->Vertices.assign(&v[0], &v[24]);

		//
		// Create the indices.
		//

		uint32 i[36];

		// Fill in the front face index data
		i[0] = 0; i[1] = 1; i[2] = 2;
		i[3] = 0; i[4] = 2; i[5] = 3;

		// Fill in the back face index data
		i[6] = 4; i[7] = 5; i[8] = 6;
		i[9] = 4; i[10] = 6; i[11] = 7;

		// Fill in the top face index data
		i[12] = 8; i[13] = 9; i[14] = 10;
		i[15] = 8; i[16] = 10; i[17] = 11;

		// Fill in the bottom face index data
		i[18] = 12; i[19] = 13; i[20] = 14;
		i[21] = 12; i[22] = 14; i[23] = 15;

		// Fill in the left face index data
		i[24] = 16; i[25] = 17; i[26] = 18;
		i[27] = 16; i[28] = 18; i[29] = 19;

		// Fill in the right face index data
		i[30] = 20; i[31] = 21; i[32] = 22;
		i[33] = 20; i[34] = 22; i[35] = 23;

		pMeshData->Indices32.assign(&i[0], &i[36]);

		// Put a cap on the number of subdivisions.
		numSubdivisions = std::min<uint32>(numSubdivisions, 6u);

		for (uint32 i = 0; i < numSubdivisions; ++i)
		{
			Subdivide(*pMeshData);
		}

		return;
	}

	//static
	// radius		- 半径
	// sliceCount	- 切片数量（垂直切割片数）
	// stackCount	- 堆叠层数（横向切割层数）
	void MeshFactory::CreateSphere(float radius, uint32 sliceCount, uint32 stackCount, MeshData* pMeshData)
	{
		pMeshData->Clear();

		//
		// Compute the vertices stating at the top pole and moving down the stacks.
		//

		// Poles: note that there will be texture coordinate distortion as there is
		// not a unique point on the texture map to assign to the pole when mapping
		// a rectangular texture onto a sphere.
		Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		Vertex btmVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

		pMeshData->Vertices.push_back(topVertex);

		float phiStep = XM_PI / stackCount;
		float thetaStep = 2.0f*XM_PI / sliceCount;

		// Compute vertices for each stack ring (do not count the poles as rings).
		for (uint32 i = 1; i <= stackCount - 1; ++i)
		{
			float phi = i * phiStep;

			// Vertices of ring.
			for (uint32 j = 0; j <= sliceCount; ++j)
			{
				float theta = j * thetaStep;

				Vertex v;

				// spherical to cartesian
				v.Position.x = radius * sinf(phi)*cosf(theta);
				v.Position.y = radius * cosf(phi);
				v.Position.z = radius * sinf(phi)*sinf(theta);

				// Partial derivative of P with respect to theta
				v.TangentU.x = -radius * sinf(phi)*sinf(theta);
				v.TangentU.y = 0.0f;
				v.TangentU.z = +radius * sinf(phi)*cosf(theta);

				XMVECTOR T = XMLoadFloat3(&v.TangentU);
				XMStoreFloat3(&v.TangentU, XMVector3Normalize(T));

				XMVECTOR p = XMLoadFloat3(&v.Position);
				XMStoreFloat3(&v.Normal, XMVector3Normalize(p));

				v.TextureC.x = theta / XM_2PI;
				v.TextureC.y = phi / XM_PI;

				pMeshData->Vertices.push_back(v);
			}
		}

		pMeshData->Vertices.push_back(btmVertex);

		//
		// Compute indices for top stack.  The top stack was written first to the vertex buffer
		// and connects the top pole to the first ring.
		//

		for (uint32 i = 1; i <= sliceCount; ++i)
		{
			pMeshData->Indices32.push_back(0);
			pMeshData->Indices32.push_back(i + 1);
			pMeshData->Indices32.push_back(i);
		}

		//
		// Compute indices for inner stacks (not connected to poles).
		//

		// Offset the indices to the index of the first vertex in the first ring.
		// This is just skipping the top pole vertex.
		uint32 baseIndex = 1;
		uint32 ringVertexCount = sliceCount + 1;
		for (uint32 i = 0; i < stackCount - 2; ++i)
		{
			for (uint32 j = 0; j < sliceCount; ++j)
			{
				pMeshData->Indices32.push_back(baseIndex + i * ringVertexCount + j);
				pMeshData->Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
				pMeshData->Indices32.push_back(baseIndex + (i + 1)*ringVertexCount + j);

				pMeshData->Indices32.push_back(baseIndex + (i + 1)*ringVertexCount + j);
				pMeshData->Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
				pMeshData->Indices32.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
			}
		}

		//
		// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
		// and connects the bottom pole to the bottom ring.
		//

		// South pole vertex was added last.
		uint32 southPoleIndex = (uint32)pMeshData->Vertices.size() - 1;

		// Offset the indices to the index of the first vertex in the last ring.
		baseIndex = southPoleIndex - ringVertexCount;

		for (uint32 i = 0; i < sliceCount; ++i)
		{
			pMeshData->Indices32.push_back(southPoleIndex);
			pMeshData->Indices32.push_back(baseIndex + i);
			pMeshData->Indices32.push_back(baseIndex + i + 1);
		}

		return;
	}

	//static
	// radius			- 半径
	// numSubdivisions	- 细分次数
	void MeshFactory::CreateGeosphere(float radius, uint32 numSubdivisions, MeshData* pMeshData)
	{
		pMeshData->Clear();

		// Put a cap on the number of subdivisions.
		numSubdivisions = std::min<uint32>(numSubdivisions, 6u);

		// Approximate a sphere by tessellating an icosahedron.

		const float X = 0.525731f;
		const float Z = 0.850651f;

		XMFLOAT3 pos[12] =
		{
			XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
			XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
			XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
			XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
			XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
			XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
		};

		uint32 k[60] =
		{
			1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
			1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
			3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
			10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
		};

		pMeshData->Vertices.resize(12);
		pMeshData->Indices32.assign(&k[0], &k[60]);

		for (uint32 i = 0; i < 12; ++i)
			pMeshData->Vertices[i].Position = pos[i];

		for (uint32 i = 0; i < numSubdivisions; ++i)
			Subdivide(*pMeshData);

		// Project vertices onto sphere and scale.
		for (uint32 i = 0; i < pMeshData->Vertices.size(); ++i)
		{
			// Project onto unit sphere.
			XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&pMeshData->Vertices[i].Position));

			// Project onto sphere.
			XMVECTOR p = radius * n;

			XMStoreFloat3(&pMeshData->Vertices[i].Position, p);
			XMStoreFloat3(&pMeshData->Vertices[i].Normal, n);

			// Derive texture coordinates from spherical coordinates.
			float theta = atan2f(pMeshData->Vertices[i].Position.z, pMeshData->Vertices[i].Position.x);

			// Put in [0, 2pi].
			if (theta < 0.0f)
				theta += XM_2PI;

			float phi = acosf(pMeshData->Vertices[i].Position.y / radius);

			pMeshData->Vertices[i].TextureC.x = theta / XM_2PI;
			pMeshData->Vertices[i].TextureC.y = phi / XM_PI;

			// Partial derivative of P with respect to theta
			pMeshData->Vertices[i].TangentU.x = -radius * sinf(phi)*sinf(theta);
			pMeshData->Vertices[i].TangentU.y = 0.0f;
			pMeshData->Vertices[i].TangentU.z = +radius * sinf(phi)*cosf(theta);

			XMVECTOR T = XMLoadFloat3(&pMeshData->Vertices[i].TangentU);
			XMStoreFloat3(&pMeshData->Vertices[i].TangentU, XMVector3Normalize(T));
		}

		return;
	}

	//static
	// btmRadius	- 底面半径
	// topRadius	- 顶面半径
	// height		- 高度 y
	// sliceCount	- 切片数量（垂直切割片数）
	// stackCount	- 堆叠层数（横向切割层数）
	void MeshFactory::CreateCylinder(float btmRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData* pMeshData)
	{
		pMeshData->Clear();

		// Build Stacks.

		float stackHeight = height / stackCount;

		// Amount to increment radius as we move up each stack level from bottom to top.
		float radiusStep = (topRadius - btmRadius) / stackCount;

		uint32 ringCount = stackCount + 1;

		// Compute vertices for each stack ring starting at the bottom and moving up.
		for (uint32 i = 0; i < ringCount; ++i)
		{
			float y = -0.5f*height + i * stackHeight;
			float r = btmRadius + i * radiusStep;

			// vertices of ring
			float dTheta = 2.0f*XM_PI / sliceCount;
			for (uint32 j = 0; j <= sliceCount; ++j)
			{
				Vertex vertex;

				float c = cosf(j*dTheta);
				float s = sinf(j*dTheta);

				vertex.Position = XMFLOAT3(r*c, y, r*s);

				vertex.TextureC.x = (float)j / sliceCount;
				vertex.TextureC.y = 1.0f - (float)i / stackCount;

				// Cylinder can be parameterized as follows, where we introduce v
				// parameter that goes in the same direction as the v tex-coord
				// so that the bitangent goes in the same direction as the v tex-coord.
				//   Let r0 be the bottom radius and let r1 be the top radius.
				//   y(v) = h - hv for v in [0,1].
				//   r(v) = r1 + (r0-r1)v
				//
				//   x(t, v) = r(v)*cos(t)
				//   y(t, v) = h - hv
				//   z(t, v) = r(v)*sin(t)
				// 
				//  dx/dt = -r(v)*sin(t)
				//  dy/dt = 0
				//  dz/dt = +r(v)*cos(t)
				//
				//  dx/dv = (r0-r1)*cos(t)
				//  dy/dv = -h
				//  dz/dv = (r0-r1)*sin(t)

				// This is unit length.
				vertex.TangentU = XMFLOAT3(-s, 0.0f, c);

				float dr = btmRadius - topRadius;
				XMFLOAT3 bitangent(dr*c, -height, dr*s);

				XMVECTOR T = XMLoadFloat3(&vertex.TangentU);
				XMVECTOR B = XMLoadFloat3(&bitangent);
				XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
				XMStoreFloat3(&vertex.Normal, N);

				pMeshData->Vertices.push_back(vertex);
			}
		}

		// Add one because we duplicate the first and last vertex per ring
		// since the texture coordinates are different.
		uint32 ringVertexCount = sliceCount + 1;

		// Compute indices for each stack.
		for (uint32 i = 0; i < stackCount; ++i)
		{
			for (uint32 j = 0; j < sliceCount; ++j)
			{
				pMeshData->Indices32.push_back(i*ringVertexCount + j);
				pMeshData->Indices32.push_back((i + 1)*ringVertexCount + j);
				pMeshData->Indices32.push_back((i + 1)*ringVertexCount + j + 1);

				pMeshData->Indices32.push_back(i*ringVertexCount + j);
				pMeshData->Indices32.push_back((i + 1)*ringVertexCount + j + 1);
				pMeshData->Indices32.push_back(i*ringVertexCount + j + 1);
			}
		}

		BuildCylinderTopCap(btmRadius, topRadius, height, sliceCount, stackCount, *pMeshData);
		BuildCylinderBottomCap(btmRadius, topRadius, height, sliceCount, stackCount, *pMeshData);

		return;
	}

	// Cone - 圆锥
	void MeshFactory::CreateCone(float radius, float height, uint32 sliceCount, uint32 stackCount, MeshData* pMeshData)
	{
		CreateCylinder(radius, 0.0f, height, sliceCount, stackCount, pMeshData);
	}

	// Tetrahedron - 四面体
	void MeshFactory::CreateTetrahedron(float size, MeshData* pMeshData)
	{
		pMeshData->Clear();

		const float SQRT2 = 1.41421356237309504880f;
		const float SQRT3 = 1.73205080756887729352f;
		const float SQRT6 = 2.44948974278317809820f;

		static const XMVECTORF32 verts[4] =
		{
			{ { {              0.f,          0.f,        1.f, 0 } } },
			{ { {  2.f*SQRT2 / 3.f,          0.f, -1.f / 3.f, 0 } } },
			{ { {     -SQRT2 / 3.f,  SQRT6 / 3.f, -1.f / 3.f, 0 } } },
			{ { {     -SQRT2 / 3.f, -SQRT6 / 3.f, -1.f / 3.f, 0 } } }
		};

		static const uint32_t faces[4 * 3] =
		{
			0, 1, 2,
			0, 2, 3,
			0, 3, 1,
			1, 3, 2,
		};

		for (size_t j = 0; j < _countof(faces); j += 3)
		{
			uint32_t v0 = faces[j];
			uint32_t v1 = faces[j + 1];
			uint32_t v2 = faces[j + 2];

			XMVECTOR normal = XMVector3Cross(verts[v1].v - verts[v0].v, verts[v2].v - verts[v0].v);
			normal = XMVector3Normalize(normal);

			size_t base = pMeshData->VertexCount();
			pMeshData->index_push_back(base + 0);
			pMeshData->index_push_back(base + 1);
			pMeshData->index_push_back(base + 2);

			// Duplicate vertices to use face normals
			Vertex vet;
			XMVECTOR position = XMVectorScale(verts[v0], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, g_XMZero);
			XMStoreFloat4(&vet.Color, Colors::Red);
			pMeshData->Vertices.push_back(vet);

			position = XMVectorScale(verts[v1], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, g_XMIdentityR0);
			XMStoreFloat4(&vet.Color, Colors::Green);
			pMeshData->Vertices.push_back(vet);

			position = XMVectorScale(verts[v2], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, g_XMIdentityR1);
			XMStoreFloat4(&vet.Color, Colors::Blue);
			pMeshData->Vertices.push_back(vet);
		}

		assert(pMeshData->VertexCount() == 4 * 3);
		assert(pMeshData->IndexCount() == 4 * 3);
	}

	// 八面体
	void MeshFactory::CreateOctahedron(float size, MeshData* pMeshData)
	{
		pMeshData->Clear();

		static const XMVECTORF32 verts[6] =
		{
			{ { {  1,  0,  0, 0 } } },
			{ { { -1,  0,  0, 0 } } },
			{ { {  0,  1,  0, 0 } } },
			{ { {  0, -1,  0, 0 } } },
			{ { {  0,  0,  1, 0 } } },
			{ { {  0,  0, -1, 0 } } }
		};

		static const uint32_t faces[8 * 3] =
		{
			4, 0, 2,
			4, 2, 1,
			4, 1, 3,
			4, 3, 0,
			5, 2, 0,
			5, 1, 2,
			5, 3, 1,
			5, 0, 3
		};

		for (size_t j = 0; j < _countof(faces); j += 3)
		{
			uint32_t v0 = faces[j];
			uint32_t v1 = faces[j + 1];
			uint32_t v2 = faces[j + 2];

			XMVECTOR normal = XMVector3Cross(verts[v1].v - verts[v0].v, verts[v2].v - verts[v0].v);
			normal = XMVector3Normalize(normal);

			size_t base = pMeshData->VertexCount();
			pMeshData->index_push_back(base + 0);
			pMeshData->index_push_back(base + 1);
			pMeshData->index_push_back(base + 2);

			// Duplicate vertices to use face normals
			Vertex vet;
			XMVECTOR position = XMVectorScale(verts[v0], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, g_XMZero);
			XMStoreFloat4(&vet.Color, Colors::Red);
			pMeshData->Vertices.push_back(vet);

			position = XMVectorScale(verts[v1], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, g_XMIdentityR0);
			XMStoreFloat4(&vet.Color, Colors::Green);
			pMeshData->Vertices.push_back(vet);

			position = XMVectorScale(verts[v2], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, g_XMIdentityR1);
			XMStoreFloat4(&vet.Color, Colors::Blue);
			pMeshData->Vertices.push_back(vet);
		}

		assert(pMeshData->VertexCount() == 8 * 3);
		assert(pMeshData->IndexCount() == 8 * 3);
	}

	// Dodecahedron - 十二面体
	void MeshFactory::CreateDodecahedron(float size, MeshData* pMeshData)
	{
		pMeshData->Clear();

		static const float a = 1.f / sqrtf(3);				// SQRT3;
		static const float b = 0.356822089773089931942f;	// sqrt( ( 3 - sqrt(5) ) / 6 )
		static const float c = 0.934172358962715696451f;	// sqrt( ( 3 + sqrt(5) ) / 6 );

		static const XMVECTORF32 verts[20] =
		{
			{ { {  a,  a,  a, 0 } } },
			{ { {  a,  a, -a, 0 } } },
			{ { {  a, -a,  a, 0 } } },
			{ { {  a, -a, -a, 0 } } },
			{ { { -a,  a,  a, 0 } } },
			{ { { -a,  a, -a, 0 } } },
			{ { { -a, -a,  a, 0 } } },
			{ { { -a, -a, -a, 0 } } },
			{ { {  b,  c,  0, 0 } } },
			{ { { -b,  c,  0, 0 } } },
			{ { {  b, -c,  0, 0 } } },
			{ { { -b, -c,  0, 0 } } },
			{ { {  c,  0,  b, 0 } } },
			{ { {  c,  0, -b, 0 } } },
			{ { { -c,  0,  b, 0 } } },
			{ { { -c,  0, -b, 0 } } },
			{ { {  0,  b,  c, 0 } } },
			{ { {  0, -b,  c, 0 } } },
			{ { {  0,  b, -c, 0 } } },
			{ { {  0, -b, -c, 0 } } }
		};

		static const uint32_t faces[12 * 5] =
		{
			0, 8, 9, 4, 16,
			0, 16, 17, 2, 12,
			12, 2, 10, 3, 13,
			9, 5, 15, 14, 4,
			3, 19, 18, 1, 13,
			7, 11, 6, 14, 15,
			0, 12, 13, 1, 8,
			8, 1, 18, 5, 9,
			16, 4, 14, 6, 17,
			6, 11, 10, 2, 17,
			7, 15, 5, 18, 19,
			7, 19, 3, 10, 11,
		};

		static const XMVECTORF32 textureCoordinates[5] =
		{
			{ { {  0.654508f, 0.0244717f, 0, 0 } } },
			{ { { 0.0954915f,  0.206107f, 0, 0 } } },
			{ { { 0.0954915f,  0.793893f, 0, 0 } } },
			{ { {  0.654508f,  0.975528f, 0, 0 } } },
			{ { {        1.f,       0.5f, 0, 0 } } }
		};

		static const uint32_t textureIndex[12][5] =
		{
			{ 0, 1, 2, 3, 4 },
			{ 2, 3, 4, 0, 1 },
			{ 4, 0, 1, 2, 3 },
			{ 1, 2, 3, 4, 0 },
			{ 2, 3, 4, 0, 1 },
			{ 0, 1, 2, 3, 4 },
			{ 1, 2, 3, 4, 0 },
			{ 4, 0, 1, 2, 3 },
			{ 4, 0, 1, 2, 3 },
			{ 1, 2, 3, 4, 0 },
			{ 0, 1, 2, 3, 4 },
			{ 2, 3, 4, 0, 1 },
		};

		size_t t = 0;
		for (size_t j = 0; j < _countof(faces); j += 5, ++t)
		{
			uint32_t v0 = faces[j];
			uint32_t v1 = faces[j + 1];
			uint32_t v2 = faces[j + 2];
			uint32_t v3 = faces[j + 3];
			uint32_t v4 = faces[j + 4];

			XMVECTOR normal = XMVector3Cross(verts[v1].v - verts[v0].v, verts[v2].v - verts[v0].v);
			normal = XMVector3Normalize(normal);

			//pMeshData
			size_t base = pMeshData->VertexCount();

			pMeshData->index_push_back(base + 0);
			pMeshData->index_push_back(base + 1);
			pMeshData->index_push_back(base + 2);

			pMeshData->index_push_back(base + 0);
			pMeshData->index_push_back(base + 2);
			pMeshData->index_push_back(base + 3);

			pMeshData->index_push_back(base + 0);
			pMeshData->index_push_back(base + 3);
			pMeshData->index_push_back(base + 4);

			// Duplicate vertices to use face normals
			Vertex vet;
			XMVECTOR position = XMVectorScale(verts[v0], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, textureCoordinates[textureIndex[t][0]]);
			XMStoreFloat4(&vet.Color, Colors::Red);
			pMeshData->vertex_push_bak(vet);

			position = XMVectorScale(verts[v1], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, textureCoordinates[textureIndex[t][1]]);
			XMStoreFloat4(&vet.Color, Colors::Green);
			pMeshData->vertex_push_bak(vet);

			position = XMVectorScale(verts[v2], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, textureCoordinates[textureIndex[t][2]]);
			XMStoreFloat4(&vet.Color, Colors::Blue);
			pMeshData->vertex_push_bak(vet);

			position = XMVectorScale(verts[v3], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, textureCoordinates[textureIndex[t][3]]);
			XMStoreFloat4(&vet.Color, Colors::Cyan);
			pMeshData->vertex_push_bak(vet);

			position = XMVectorScale(verts[v4], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, textureCoordinates[textureIndex[t][4]]);
			XMStoreFloat4(&vet.Color, Colors::Beige);
			pMeshData->vertex_push_bak(vet);
		}

		assert(pMeshData->VertexCount() == 12 * 5);
		assert(pMeshData->IndexCount() == 12 * 3 * 3);
	}


	// Icosahedron - 二十面体
	// size - 尺寸 1.0
	void MeshFactory::CreateIcosahedron(float size, MeshData* pMeshData)
	{
		pMeshData->Clear();

		static const float  t = 1.618033988749894848205f; // (1 + sqrt(5)) / 2
		static const float t2 = 1.519544995837552493271f; // sqrt( 1 + sqr( (1 + sqrt(5)) / 2 ) )

		static const XMVECTORF32 verts[12] =
		{
			{ { {    t / t2,  1.f / t2,       0, 0 } } },
			{ { {   -t / t2,  1.f / t2,       0, 0 } } },
			{ { {    t / t2, -1.f / t2,       0, 0 } } },
			{ { {   -t / t2, -1.f / t2,       0, 0 } } },
			{ { {  1.f / t2,       0,    t / t2, 0 } } },
			{ { {  1.f / t2,       0,   -t / t2, 0 } } },
			{ { { -1.f / t2,       0,    t / t2, 0 } } },
			{ { { -1.f / t2,       0,   -t / t2, 0 } } },
			{ { {       0,    t / t2,  1.f / t2, 0 }  } },
			{ { {       0,   -t / t2,  1.f / t2, 0 } } },
			{ { {       0,    t / t2, -1.f / t2, 0 } } },
			{ { {       0,   -t / t2, -1.f / t2, 0 } } }
		};

		static const uint32_t faces[20 * 3] =
		{
			0, 8, 4,
			0, 5, 10,
			2, 4, 9,
			2, 11, 5,
			1, 6, 8,
			1, 10, 7,
			3, 9, 6,
			3, 7, 11,
			0, 10, 8,
			1, 8, 10,
			2, 9, 11,
			3, 11, 9,
			4, 2, 0,
			5, 0, 2,
			6, 1, 3,
			7, 3, 1,
			8, 6, 4,
			9, 4, 6,
			10, 5, 7,
			11, 7, 5
		};

		for (size_t j = 0; j < _countof(faces); j += 3)
		{
			uint32_t v0 = faces[j + 0];
			uint32_t v1 = faces[j + 1];
			uint32_t v2 = faces[j + 2];
			//uint32_t v[3] = { faces[j + 0] ,faces[j + 1] ,faces[j + 2] };

			XMVECTOR normal = XMVector3Cross(verts[v1].v - verts[v0].v,
				verts[v2].v - verts[v0].v);

			normal = XMVector3Normalize(normal);

			size_t base = pMeshData->VertexCount();
			pMeshData->index_push_back(base + 0);
			pMeshData->index_push_back(base + 1);
			pMeshData->index_push_back(base + 2);

			// Duplicate vertices to use face normals
			Vertex vet;
			XMVECTOR position = XMVectorScale(verts[v0], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, g_XMZero);
			XMStoreFloat4(&vet.Color, Colors::Red);
			pMeshData->Vertices.push_back(vet);

			position = XMVectorScale(verts[v1], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, g_XMIdentityR0);
			XMStoreFloat4(&vet.Color, Colors::Green);
			pMeshData->Vertices.push_back(vet);

			position = XMVectorScale(verts[v2], size);
			XMStoreFloat3(&vet.Position, position);
			XMStoreFloat3(&vet.Normal, normal);
			XMStoreFloat2(&vet.TextureC, g_XMIdentityR1);
			XMStoreFloat4(&vet.Color, Colors::Blue);
			pMeshData->Vertices.push_back(vet);
		}

		assert(pMeshData->VertexCount() == 20 * 3);
		assert(pMeshData->IndexCount() == 20 * 3);

		return;
	}

	// Torus - 圆环面
	// diameter		- 直径	2.0
	// thickness	- 厚度	1.0
	// tessellation	- 细分	30
	void MeshFactory::CreateTorus(float diameter, float thickness, size_t tessellation, MeshData* pMeshData)
	{
		pMeshData->Clear();

		if (tessellation < 3)
		{
			tessellation = 3;
			//throw std::out_of_range("tesselation parameter out of range");
		}

		size_t stride = tessellation + 1;

		// First we loop around the main ring of the torus.
		for (size_t i = 0; i <= tessellation; i++)
		{
			float u = (float)i / tessellation;

			float outerAngle = i * XM_2PI / tessellation - XM_PIDIV2;

			// Create a transform matrix that will align geometry to
			// slice perpendicularly though the current ring position.
			XMMATRIX transform = XMMatrixTranslation(diameter / 2, 0, 0) * XMMatrixRotationY(outerAngle);

			// Now we loop along the other axis, around the side of the tube.
			for (size_t j = 0; j <= tessellation; j++)
			{
				float v = 1 - (float)j / tessellation;

				float innerAngle = j * XM_2PI / tessellation + XM_PI;
				float dx, dy;

				XMScalarSinCos(&dy, &dx, innerAngle);

				// Create a vertex.
				XMVECTOR normal = XMVectorSet(dx, dy, 0, 0);
				XMVECTOR position = normal * thickness / 2;
				XMVECTOR textureCoordinate = XMVectorSet(u, v, 0, 0);

				position = XMVector3Transform(position, transform);
				normal = XMVector3TransformNormal(normal, transform);

				Vertex vet;
				XMStoreFloat3(&vet.Position, position);
				XMStoreFloat3(&vet.Normal, normal);
				XMStoreFloat2(&vet.TextureC, textureCoordinate);
				//XMStoreFloat4(&vet.Color, Colors::Red);
				pMeshData->Vertices.push_back(vet);

				// And create indices for two triangles.
				size_t nextI = (i + 1) % stride;
				size_t nextJ = (j + 1) % stride;

				pMeshData->index_push_back(i * stride + j);
				pMeshData->index_push_back(i * stride + nextJ);
				pMeshData->index_push_back(nextI * stride + j);

				pMeshData->index_push_back(i * stride + nextJ);
				pMeshData->index_push_back(nextI * stride + nextJ);
				pMeshData->index_push_back(nextI * stride + j);
			}
		}

		pMeshData->ReverseWinding();

		return;
	}



#pragma region --Private Functions

	//static
	void MeshFactory::Subdivide(MeshData& meshData)
	{
		// Save a copy of the input geometry.
		MeshData inputCopy = meshData;


		meshData.Vertices.resize(0);
		meshData.Indices32.resize(0);

		//       v1
		//       *
		//      / \
		//     /   \
		//  m0*-----*m1
		//   / \   / \
		//  /   \ /   \
		// *-----*-----*
		// v0    m2     v2

		uint32 numTris = (uint32)inputCopy.Indices32.size() / 3;
		for (uint32 i = 0; i < numTris; ++i)
		{
			Vertex v0 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 0]];
			Vertex v1 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 1]];
			Vertex v2 = inputCopy.Vertices[inputCopy.Indices32[i * 3 + 2]];

			//
			// Generate the midpoints.
			//

			Vertex m0 = MidPoint(v0, v1);
			Vertex m1 = MidPoint(v1, v2);
			Vertex m2 = MidPoint(v0, v2);

			//
			// Add new geometry.
			//

			meshData.Vertices.push_back(v0); // 0
			meshData.Vertices.push_back(v1); // 1
			meshData.Vertices.push_back(v2); // 2
			meshData.Vertices.push_back(m0); // 3
			meshData.Vertices.push_back(m1); // 4
			meshData.Vertices.push_back(m2); // 5

			meshData.Indices32.push_back(i * 6 + 0);
			meshData.Indices32.push_back(i * 6 + 3);
			meshData.Indices32.push_back(i * 6 + 5);

			meshData.Indices32.push_back(i * 6 + 3);
			meshData.Indices32.push_back(i * 6 + 4);
			meshData.Indices32.push_back(i * 6 + 5);

			meshData.Indices32.push_back(i * 6 + 5);
			meshData.Indices32.push_back(i * 6 + 4);
			meshData.Indices32.push_back(i * 6 + 2);

			meshData.Indices32.push_back(i * 6 + 3);
			meshData.Indices32.push_back(i * 6 + 1);
			meshData.Indices32.push_back(i * 6 + 4);
		}
	}

	//static
	Vertex MeshFactory::MidPoint(const Vertex& v0, const Vertex& v1)
	{
		XMVECTOR p0 = XMLoadFloat3(&v0.Position);
		XMVECTOR p1 = XMLoadFloat3(&v1.Position);

		XMVECTOR n0 = XMLoadFloat3(&v0.Normal);
		XMVECTOR n1 = XMLoadFloat3(&v1.Normal);

		XMVECTOR tan0 = XMLoadFloat3(&v0.TangentU);
		XMVECTOR tan1 = XMLoadFloat3(&v1.TangentU);

		XMVECTOR tex0 = XMLoadFloat2(&v0.TextureC);
		XMVECTOR tex1 = XMLoadFloat2(&v1.TextureC);

		// Compute the midpoints of all the attributes.  Vectors need to be normalized
		// since linear interpolating can make them not unit length.  
		XMVECTOR pos = 0.5f*(p0 + p1);
		XMVECTOR normal = XMVector3Normalize(0.5f*(n0 + n1));
		XMVECTOR tangent = XMVector3Normalize(0.5f*(tan0 + tan1));
		XMVECTOR tex = 0.5f*(tex0 + tex1);

		Vertex v;
		XMStoreFloat3(&v.Position, pos);
		XMStoreFloat3(&v.Normal, normal);
		XMStoreFloat3(&v.TangentU, tangent);
		XMStoreFloat2(&v.TextureC, tex);

		return v;
	}

	//static
	void MeshFactory::BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData)
	{
		uint32 baseIndex = (uint32)meshData.Vertices.size();

		float y = 0.5f*height;
		float dTheta = 2.0f*XM_PI / sliceCount;

		// Duplicate cap ring vertices because the texture coordinates and normals differ.
		for (uint32 i = 0; i <= sliceCount; ++i)
		{
			float x = topRadius * cosf(i*dTheta);
			float z = topRadius * sinf(i*dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			meshData.Vertices.push_back(Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
		}

		// Cap center vertex.
		meshData.Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

		// Index of center vertex.
		uint32 centerIndex = (uint32)meshData.Vertices.size() - 1;

		for (uint32 i = 0; i < sliceCount; ++i)
		{
			meshData.Indices32.push_back(centerIndex);
			meshData.Indices32.push_back(baseIndex + i + 1);
			meshData.Indices32.push_back(baseIndex + i);
		}
	}

	//static
	void MeshFactory::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData)
	{
		// 
		// Build bottom cap.
		//

		uint32 baseIndex = (uint32)meshData.Vertices.size();
		float y = -0.5f*height;

		// vertices of ring
		float dTheta = 2.0f*XM_PI / sliceCount;
		for (uint32 i = 0; i <= sliceCount; ++i)
		{
			float x = bottomRadius * cosf(i*dTheta);
			float z = bottomRadius * sinf(i*dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			meshData.Vertices.push_back(Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
		}

		// Cap center vertex.
		meshData.Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

		// Cache the index of center vertex.
		uint32 centerIndex = (uint32)meshData.Vertices.size() - 1;

		for (uint32 i = 0; i < sliceCount; ++i)
		{
			meshData.Indices32.push_back(centerIndex);
			meshData.Indices32.push_back(baseIndex + i);
			meshData.Indices32.push_back(baseIndex + i + 1);
		}
	}

#pragma endregion


}//DX12

