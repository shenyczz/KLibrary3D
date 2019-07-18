#include "stdafx.h"
#include "MeshFactory.h"

void t()
{
	//MeshFactory::CreateTriangle<Vertex_tri>(&pMesh);
}

//static
void MeshFactory::CreateTriangle(const float side, MeshData* pMesh)
{
	pMesh->Vertices.clear();
	pMesh->Indices.clear();

	float slen = side;

	// ���֣�ԭ��������������
	/*
	                p1(0.5f, 1.0f ,0.0f)
	                  /\
	                 /  \
	                /    \
				   /	  \
				  /	       \
				 /		    \
				/			 \
			   /--------------\
	    p0(0,0, 0)          p2(1, 0, 0)

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

	std::vector<XMFLOAT4> clrs =
	{
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
	};

	std::vector<XMFLOAT3> noms =
	{
		XMFLOAT3(+0.0f, +0.0f, +0.0f),
		XMFLOAT3(+0.0f, +0.0f, +0.0f),
		XMFLOAT3(+0.0f, +0.0f, +0.0f),
	};

	std::vector<XMFLOAT3> tags =
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

	// ��������
	for (int i = 0; i < 3; i++)
	{
		XMFLOAT3 pnt = { pnts[i].x, pnts[i].y, pnts[i].z };
		pMesh->Vertices.push_back(Vertex(pnts[i], clrs[i], noms[i], tags[i], texs[i]));
	}

	// ��������
	pMesh->Indices =
	{
		0, 1, 2,
	};

	return;
}

// static
void MeshFactory::CreateQuad(float width, float height, MeshData* pMesh)
{
	pMesh->Vertices.clear();
	pMesh->Indices.clear();

	float w = width;
	float h = height;
	float d = 0;

	// ���֣�ԭ��������������

	//	v1(-1, +1, 0)			v2(1, 1, 0)
	//			------------------
	//			|				 |
	//			|				 |
	//			|	 (0, 0, 0)	 |
	//			|				 |
	//			|				 |
	//			------------------
	//	v0(-1, -1, 0)			v3(1, -1, 0) 
	//
	std::vector<XMFLOAT3> pnts =
	{
		// p0-p1-p2-p3
		XMFLOAT3(-1.0f, -1.0f, 0.0f),
		XMFLOAT3(-1.0f, +1.0f, 0.0f),
		XMFLOAT3(+1.0f, +1.0f, 0.0f),
		XMFLOAT3(+1.0f, -1.0f, 0.0f),
	};

	std::vector<XMFLOAT4> clrs =
	{
		// p0-p1-p2-p3
		XMFLOAT4(1.0f, 0.0f, 0.0f,1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f,1.0f),
		XMFLOAT4(1.0f, 1.0f, 0.0f,1.0f),
		XMFLOAT4(0.0f, 1.0f, 1.0f,1.0f),
	};

	std::vector<XMFLOAT3> noms =
	{
		// p0-p1-p2-p3
		XMFLOAT3(+0.0f, +0.0f, 0.0f),
		XMFLOAT3(+0.0f, +0.0f, 0.0f),
		XMFLOAT3(+0.0f, +0.0f, 0.0f),
		XMFLOAT3(+0.0f, +0.0f, 0.0f),
	};

	std::vector<XMFLOAT3> tags =
	{
		// p0-p1-p2-p3
		XMFLOAT3(+0.0f, +0.0f, 0.0f),
		XMFLOAT3(+0.0f, +0.0f, 0.0f),
		XMFLOAT3(+0.0f, +0.0f, 0.0f),
		XMFLOAT3(+0.0f, +0.0f, 0.0f),
	};

	std::vector<XMFLOAT2> texs =
	{
		// p0-p1-p2-p3
		XMFLOAT2(0.5f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(0.0f, 0.5f),
	};

	// ��������
	for (int i = 0; i < 4; i++)
	{
		XMFLOAT3 pnt = { pnts[i].x*w, pnts[i].y*h, pnts[i].z*d };
		pMesh->Vertices.push_back(Vertex(pnt, clrs[i], noms[i], tags[i], texs[i]));
	}

	// ��������(˳ʱ��)
	pMesh->Indices =
	{
		0, 1, 2,
		0, 2, 3
	};


	return;
}


// static ����һ�������壺ָ����(X����)����(Y����)����(Z����)
void MeshFactory::CreateBox(float width, float height, float depth, MeshData* pMesh)
{
	pMesh->Vertices.clear();
	pMesh->Indices.clear();


	
	//////////////////////////////////////////////////////////////////////////////////////////
	//
	//             v5---------------v6
	//		      / |               /|
	//           /  |              / |
	//		   v1----------------v2  |
	//			|	|            |	 |
	//			|	v4-----------|---v7
	//			|  /	         |	/
	//			| /				 | /	
	//			|/				 |/	
	//		   v0----------------v3
	//
	//
	//	v1(-1, +1, -1)			v2(1, 1, -1)		v5(-1, +1, 1)			v6(1, 1, 1)
	//			------------------							------------------
	//			|				 |							|				 |
	//			|				 |							|				 |
	//			|	             |							|	             |
	//			|				 |							|				 |
	//			|				 |							|				 |
	//			------------------							------------------
	//	v0(-1, -1, -1)			v3(1, -1, -1) 		v4(-1, -1, 1)			v7(1, -1, 1) 
	//
	//////////////////////////////////////////////////////////////////////////////////////////

	// 8�����㣬6���棬36������

	// ���֣�ԭ��������������
	// ʹ���������꣺X�ң�Y�ϣ�Zǰ���۾�����Z��������

	// 8������
	std::vector<XMFLOAT3> pnts =
	{
		// ǰ���ı���(���½�+˳ʱ��) 0 -> 1 -> 2 -> 3
		XMFLOAT3(-0.5f, -0.5f, +0.25f),
		XMFLOAT3(-0.5f, +0.5f, +0.25f),
		XMFLOAT3(+0.5f, +0.5f, +0.25f),
		XMFLOAT3(+0.5f, -0.5f, +0.25f),

		// �������ı���(���½�+˳ʱ��) 4 -> 5 -> 6 -> 7
		XMFLOAT3(-0.5f + 0.15f, -0.5f + 0.3f, +0.5f),
		XMFLOAT3(-0.5f + 0.15f, +0.5f + 0.3f, +0.5f),
		XMFLOAT3(+0.5f + 0.15f, +0.5f + 0.3f, +0.5f),
		XMFLOAT3(+0.5f + 0.15f, -0.5f + 0.3f, +0.5f),
	};

	std::vector<XMFLOAT4> clrs =
	{
		// ǰ���ı���(���½�+˳ʱ��) 0 -> 1 -> 2 -> 3
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f),

		// �������ı���(���½�+˳ʱ��) 4 -> 5 -> 6 -> 7
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f),
	};

	float w = 0.5f * width;
	float h = 0.5f * height;
	float d = 0.5f * depth;

	int nCount = pnts.size();

	// ��������
	for (int i = 0; i < nCount; i++)
	{
		XMFLOAT3 pnt = { pnts[i].x*w, pnts[i].y*h, pnts[i].z*d };
		//pMesh->Vertices.push_back(Vertex(pnt));
		pMesh->Vertices.push_back(Vertex(pnt, clrs[i]));
	}

	// ��������(˳ʱ��)
	std::vector<std::uint32_t> indices1 =
	{
		// 1.��ǰ��(˳)
		0, 1, 2, 0, 2, 3,	//˳
		//0, 2, 1, 0, 3, 2,	//��

		// 2.�����(��)
		0, 4, 5, 0, 5, 1,	//��
		//0, 1, 5, 0, 5, 4,	//˳

		// 3.������(��)
		4, 6, 5, 4, 7, 6,	//��
		//4, 5, 6, 4, 6, 7,	//˳

		// 4.�Ҳ���(˳)
		2, 6, 7, 2, 7, 3,	//˳
		//2, 3, 7, 2, 7, 6,	//��

		// 5.�϶���(˳)
		1, 5, 6, 1, 6, 2,	//˳
		//1, 2, 5, 1, 2, 6,	//��

		// 6.�µ���(��)
		3, 7, 4, 3, 4, 0,	//��
		//3, 4, 7, 3, 0, 4,	//˳
	};

	// ��������(˳ʱ��)
	std::vector<std::uint32_t> indices2 =
	{
		// 1.��ǰ��(��)
		//0, 1, 2, 0, 2, 3,	//˳
		0, 2, 1, 0, 3, 2,	//��

		// 2.�����(��)
		//0, 4, 5, 0, 5, 1,	//��
		0, 1, 5, 0, 5, 4,	//˳

		// 3.������(��)
		//4, 6, 5, 4, 7, 6,	//��
		4, 5, 6, 4, 6, 7,	//˳

		// 4.�Ҳ���(˳)
		//2, 6, 7, 2, 7, 3,	//˳
		2, 3, 7, 2, 7, 6,	//��

		// 5.�϶���(˳)
		//1, 5, 6, 1, 6, 2,	//˳
		1, 2, 5, 1, 2, 6,	//��

		// 6.�µ���(��)
		//3, 7, 4, 3, 4, 0,	//��
		3, 4, 7, 3, 0, 4,	//˳
	};

	for (size_t i = 0; i < 6*6; i++)
	{
		pMesh->Indices.push_back(indices1[i]);	//˳
	}


	return;
}

/*
	void CreateBox(float width, float height, float depth, MeshData &pMesh)
	{
		//ǰ��
		pMesh.vertices[0].pos = XMFLOAT3(-halfW,-halfH,-halfD);
		pMesh.vertices[0].normal = XMFLOAT3(0.f,0.f,-1.f);
		pMesh.vertices[0].tangent = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[0].tex = XMFLOAT2(0.f,1.f);
		pMesh.vertices[1].pos = XMFLOAT3(-halfW,halfH,-halfD);
		pMesh.vertices[1].normal = XMFLOAT3(0.f,0.f,-1.f);
		pMesh.vertices[1].tangent = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[1].tex = XMFLOAT2(0.f,0.f);
		pMesh.vertices[2].pos = XMFLOAT3(halfW,halfH,-halfD);
		pMesh.vertices[2].normal = XMFLOAT3(0.f,0.f,-1.f);
		pMesh.vertices[2].tangent = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[2].tex = XMFLOAT2(1.f,0.f);
		pMesh.vertices[3].pos = XMFLOAT3(halfW,-halfH,-halfD);
		pMesh.vertices[3].normal = XMFLOAT3(0.f,0.f,-1.f);
		pMesh.vertices[3].tangent = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[3].tex = XMFLOAT2(1.f,1.f);

		//�����
		pMesh.vertices[4].pos = XMFLOAT3(-halfW,-halfH,halfD);
		pMesh.vertices[4].normal = XMFLOAT3(-1.f,0.f,0.f);
		pMesh.vertices[4].tangent = XMFLOAT3(0.f,0.f,-1.f);
		pMesh.vertices[4].tex = XMFLOAT2(0.f,1.f);
		pMesh.vertices[5].pos = XMFLOAT3(-halfW,halfH,halfD);
		pMesh.vertices[5].normal = XMFLOAT3(-1.f,0.f,0.f);
		pMesh.vertices[5].tangent = XMFLOAT3(0.f,0.f,-1.f);
		pMesh.vertices[5].tex = XMFLOAT2(0.f,0.f);
		pMesh.vertices[6].pos = XMFLOAT3(-halfW,halfH,-halfD);
		pMesh.vertices[6].normal = XMFLOAT3(-1.f,0.f,0.f);
		pMesh.vertices[6].tangent = XMFLOAT3(0.f,0.f,-1.f);
		pMesh.vertices[6].tex = XMFLOAT2(1.f,0.f);
		pMesh.vertices[7].pos = XMFLOAT3(-halfW,-halfH,-halfD);
		pMesh.vertices[7].normal = XMFLOAT3(-1.f,0.f,0.f);
		pMesh.vertices[7].tangent = XMFLOAT3(0.f,0.f,-1.f);
		pMesh.vertices[7].tex = XMFLOAT2(1.f,1.f);
		//����
		pMesh.vertices[8].pos = XMFLOAT3(halfW,-halfH,halfD);
		pMesh.vertices[8].normal = XMFLOAT3(0.f,0.f,1.f);
		pMesh.vertices[8].tangent = XMFLOAT3(-1.f,0.f,0.f);
		pMesh.vertices[8].tex = XMFLOAT2(0.f,1.f);
		pMesh.vertices[9].pos = XMFLOAT3(halfW,halfH,halfD);
		pMesh.vertices[9].normal = XMFLOAT3(0.f,0.f,1.f);
		pMesh.vertices[9].tangent = XMFLOAT3(-1.f,0.f,0.f);
		pMesh.vertices[9].tex = XMFLOAT2(0.f,0.f);
		pMesh.vertices[10].pos = XMFLOAT3(-halfW,halfH,halfD);
		pMesh.vertices[10].normal = XMFLOAT3(0.f,0.f,1.f);
		pMesh.vertices[10].tangent = XMFLOAT3(-1.f,0.f,0.f);
		pMesh.vertices[10].tex = XMFLOAT2(1.f,0.f);
		pMesh.vertices[11].pos = XMFLOAT3(-halfW,-halfH,halfD);
		pMesh.vertices[11].normal = XMFLOAT3(0.f,0.f,1.f);
		pMesh.vertices[11].tangent = XMFLOAT3(-1.f,0.f,0.f);
		pMesh.vertices[11].tex = XMFLOAT2(1.f,1.f);
		//�Ҳ���
		pMesh.vertices[12].pos = XMFLOAT3(halfW,-halfH,-halfD);
		pMesh.vertices[12].normal = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[12].tangent = XMFLOAT3(0.f,0.f,1.f);
		pMesh.vertices[12].tex = XMFLOAT2(0.f,1.f);
		pMesh.vertices[13].pos = XMFLOAT3(halfW,halfH,-halfD);
		pMesh.vertices[13].normal = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[13].tangent = XMFLOAT3(0.f,0.f,1.f);
		pMesh.vertices[13].tex = XMFLOAT2(0.f,0.f);
		pMesh.vertices[14].pos = XMFLOAT3(halfW,halfH,halfD);
		pMesh.vertices[14].normal = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[14].tangent = XMFLOAT3(0.f,0.f,1.f);
		pMesh.vertices[14].tex = XMFLOAT2(1.f,0.f);
		pMesh.vertices[15].pos = XMFLOAT3(halfW,-halfH,halfD);
		pMesh.vertices[15].normal = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[15].tangent = XMFLOAT3(0.f,0.f,1.f);
		pMesh.vertices[15].tex = XMFLOAT2(1.f,1.f);
		//����
		pMesh.vertices[16].pos = XMFLOAT3(-halfW,halfH,-halfD);
		pMesh.vertices[16].normal = XMFLOAT3(0.f,1.f,0.f);
		pMesh.vertices[16].tangent = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[16].tex = XMFLOAT2(0.f,1.f);
		pMesh.vertices[17].pos = XMFLOAT3(-halfW,halfH,halfD);
		pMesh.vertices[17].normal = XMFLOAT3(0.f,1.f,0.f);
		pMesh.vertices[17].tangent = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[17].tex = XMFLOAT2(0.f,0.f);
		pMesh.vertices[18].pos = XMFLOAT3(halfW,halfH,halfD);
		pMesh.vertices[18].normal = XMFLOAT3(0.f,1.f,0.f);
		pMesh.vertices[18].tangent = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[18].tex = XMFLOAT2(1.f,0.f);
		pMesh.vertices[19].pos = XMFLOAT3(halfW,halfH,-halfD);
		pMesh.vertices[19].normal = XMFLOAT3(0.f,1.f,0.f);
		pMesh.vertices[19].tangent = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[19].tex = XMFLOAT2(1.f,1.f);
		//����
		pMesh.vertices[20].pos = XMFLOAT3(-halfW,-halfH,halfD);
		pMesh.vertices[20].normal = XMFLOAT3(0.f,-1.f,0.f);
		pMesh.vertices[20].tangent = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[20].tex = XMFLOAT2(0.f,1.f);
		pMesh.vertices[21].pos = XMFLOAT3(-halfW,-halfH,-halfD);
		pMesh.vertices[21].normal = XMFLOAT3(0.f,-1.f,0.f);
		pMesh.vertices[21].tangent = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[21].tex = XMFLOAT2(0.f,0.f);
		pMesh.vertices[22].pos = XMFLOAT3(halfW,-halfH,-halfD);
		pMesh.vertices[22].normal = XMFLOAT3(0.f,-1.f,0.f);
		pMesh.vertices[22].tangent = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[22].tex = XMFLOAT2(1.f,0.f);
		pMesh.vertices[23].pos = XMFLOAT3(halfW,-halfH,halfD);
		pMesh.vertices[23].normal = XMFLOAT3(0.f,-1.f,0.f);
		pMesh.vertices[23].tangent = XMFLOAT3(1.f,0.f,0.f);
		pMesh.vertices[23].tex = XMFLOAT2(1.f,1.f);
		
		//��������
		pMesh.indices[0] = 0;
		pMesh.indices[1] = 1;
		pMesh.indices[2] = 2;
		pMesh.indices[3] = 0;
		pMesh.indices[4] = 2;
		pMesh.indices[5] = 3;
		
		pMesh.indices[6] = 4;
		pMesh.indices[7] = 5;
		pMesh.indices[8] = 6;
		pMesh.indices[9] = 4;
		pMesh.indices[10] = 6;
		pMesh.indices[11] = 7;
		
		pMesh.indices[12] = 8;
		pMesh.indices[13] = 9;
		pMesh.indices[14] = 10;
		pMesh.indices[15] = 8;
		pMesh.indices[16] = 10;
		pMesh.indices[17] = 11;
		
		pMesh.indices[18] = 12;
		pMesh.indices[19] = 13;
		pMesh.indices[20] = 14;
		pMesh.indices[21] = 12;
		pMesh.indices[22] = 14;
		pMesh.indices[23] = 15;
		
		pMesh.indices[24] = 16;
		pMesh.indices[25] = 17;
		pMesh.indices[26] = 18;
		pMesh.indices[27] = 16;
		pMesh.indices[28] = 18;
		pMesh.indices[29] = 19;
		
		pMesh.indices[30] = 20;
		pMesh.indices[31] = 21;
		pMesh.indices[32] = 22;
		pMesh.indices[33] = 20;
		pMesh.indices[34] = 22;
		pMesh.indices[35] = 23;
	}

*/







/*
void MeshFactory::CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData)
{
    meshData.Vertices.clear();
    meshData.Indices.clear();

    // ���㶥�˵ļ��˵㣬���������ƶ���
    //

    // ���˵㣺ע����ͼ������ܻ�Ť������Ϊ��������ͼӳ�䵽���嵼��û�к��ʵ�λ��ӳ�䵽���˵㡣
    Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    meshData.Vertices.push_back( topVertex );

    float phiStep   = XM_PI/stackCount;
    float thetaStep = 2.0f*XM_PI/sliceCount;

    // ����ÿ��ջ���Ķ��㣨�������˵���Ϊ����
    for(UINT i = 1; i <= stackCount-1; ++i)
    {
        float phi = i*phiStep;

        // ���Ķ���
        for(UINT j = 0; j <= sliceCount; ++j)
        {
            float theta = j*thetaStep;

            Vertex v;

            // ���浽�ѿ�������ϵ
            v.Position.X = radius*sinf(phi)*cosf(theta);
            v.Position.Y = radius*cosf(phi);
            v.Position.Z = radius*sinf(phi)*sinf(theta);

            // Partial derivative of P with respect to theta
            v.TangentU.X = -radius*sinf(phi)*sinf(theta);
            v.TangentU.Y = 0.0f;
            v.TangentU.Z = +radius*sinf(phi)*cosf(theta);

            XMVECTOR T = XMLoadFloat3(&v.TangentU);
            XMStoreFloat3(&v.TangentU, XMVector3Normalize(T));

            XMVECTOR p = XMLoadFloat3(&v.Position);
            XMStoreFloat3(&v.Normal, XMVector3Normalize(p));

            v.TexC.X = theta / XM_2PI;
            v.TexC.Y = phi / XM_PI;

            meshData.Vertices.push_back( v );
        }
    }

    meshData.Vertices.push_back( bottomVertex );

    //
    // ����ѵ��������Ѷ��Ƕ��㻺���һ�����ݣ��������Ӷ��˵ļ��˵㵽��һ������
    //

    for(UINT i = 1; i <= sliceCount; ++i)
    {
        meshData.Indices.push_back(0);
        meshData.Indices.push_back(i+1);
        meshData.Indices.push_back(i);
    }

    //
    // �����ڶѵ������������������˵㣩

    // ��һ�����㵽��һ����������ƫ��
    // ��������������˵ļ��˶���
    UINT baseIndex = 1;
    UINT ringVertexCount = sliceCount+1;
    for(UINT i = 0; i < stackCount-2; ++i)
    {
        for(UINT j = 0; j < sliceCount; ++j)
        {
            meshData.Indices.push_back(baseIndex + i*ringVertexCount + j);
            meshData.Indices.push_back(baseIndex + i*ringVertexCount + j+1);
            meshData.Indices.push_back(baseIndex + (i+1)*ringVertexCount + j);

            meshData.Indices.push_back(baseIndex + (i+1)*ringVertexCount + j);
            meshData.Indices.push_back(baseIndex + i*ringVertexCount + j+1);
            meshData.Indices.push_back(baseIndex + (i+1)*ringVertexCount + j+1);
        }
    }

    //
    // ����׶ѵ��������׶������д�����㻺��ģ��������ӵͶ˵ļ��˵�͵׶˻�
    //

    // �ϼ��˶����������ӵ�
    UINT southPoleIndex = (UINT)meshData.Vertices.size()-1;

    // ��һ�����㵽���һ������ƫ������
    baseIndex = southPoleIndex - ringVertexCount;

    for(UINT i = 0; i < sliceCount; ++i)
    {
        meshData.Indices.push_back(southPoleIndex);
        meshData.Indices.push_back(baseIndex+i);
        meshData.Indices.push_back(baseIndex+i+1);
    }
}



*/

/*

void MeshFactory::CreateGeosphere(float radius, UINT numSubdivisions, MeshData& meshData)
{
    // Put a cap on the number of subdivisions.
    // ϸ����
    numSubdivisions = MathHelper::Min(numSubdivisions, 5u);

    // Approximate a sphere by tessellating an icosahedron.
    // ͨ��ϸ�ֶ�ʮ��������������

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

	// UINT32
    DWORD k[60] = 
    {
        1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,    
        1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,    
        3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0, 
        10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7 
    };

    meshData.Vertices.resize(12);
    meshData.Indices.resize(60);

    for(UINT i = 0; i < 12; ++i)
        meshData.Vertices[i].Position = pos[i];

    for(UINT i = 0; i < 60; ++i)
        meshData.Indices[i] = k[i];

    for(UINT i = 0; i < numSubdivisions; ++i)
        Subdivide(meshData);

    // Ͷ�䶥�㵽���岢�ҽ�������

    for(UINT i = 0; i < meshData.Vertices.size(); ++i)
    {
        // Ͷ�䵽��λ��
        XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&meshData.Vertices[i].Position));

        // Ͷ�䵽���ϰ뾶������
        XMVECTOR p = radius*n;

        XMStoreFloat3(&meshData.Vertices[i].Position, p);
        XMStoreFloat3(&meshData.Vertices[i].Normal, n);

        // ���������굼����������
        float theta = MathHelper::AngleFromXY(
            meshData.Vertices[i].Position.X, 
            meshData.Vertices[i].Position.Z);

        float phi = acosf(meshData.Vertices[i].Position.Y / radius);

        meshData.Vertices[i].TexC.X = theta/XM_2PI;
        meshData.Vertices[i].TexC.Y = phi/XM_PI;

        // Partial derivative of P with respect to theta
        meshData.Vertices[i].TangentU.X = -radius*sinf(phi)*sinf(theta);
        meshData.Vertices[i].TangentU.Y = 0.0f;
        meshData.Vertices[i].TangentU.Z = +radius*sinf(phi)*cosf(theta);

        XMVECTOR T = XMLoadFloat3(&meshData.Vertices[i].TangentU);
        XMStoreFloat3(&meshData.Vertices[i].TangentU, XMVector3Normalize(T));
    }
}



*/