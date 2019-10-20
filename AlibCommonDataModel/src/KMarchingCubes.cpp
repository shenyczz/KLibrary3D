#include "stdafx.h"
#include "KMarchingCubes.h"
#include "KMarchingCubesTriangleCases.h"


KMarchingCubes::KMarchingCubes()
{
}


KMarchingCubes::~KMarchingCubes()
{
}





//---------------------------------------------------------
//
// ��������ϵ ?
//    z
//	  |
//	  |    y
//	  |   /
//	  |  /
//	  | /
//	  0--------------x
//
// 
// �����壺8�����㡢6���桢12���ߡ�12�������Ρ�36����������
//
// scalars		- ����Ҫ��ֵ��dim = dims[0]*dims[1]*dims[2]��
// dims			- x,y,z����ά��
// origin[3]	- ���ĵ�����
// spacing[3]	- С������
// values		- Ҫ׷�ٵĵ�ֵ��ֵ
// numValues	- ��ֵ������
// 
void KMarchingCubes::Building_0(double *scalars, int dims[3], double origin[3], double spacing[3], double *values, int numValues)
{
	static int CASE_MASK[8] = { 1,2,4,8,16,32,64,128 };
	static int EDGES[12][2] =
	{ 
		{ 0,1 },{ 1,2 },{ 3,2 },{ 0,3 },
		{ 4,5 },{ 5,6 },{ 7,6 },{ 4,7 },
		{ 0,4 },{ 1,5 },{ 3,7 },{ 2,6 },
	};

	// С��������
	int iCubeNX = dims[0];
	int iCubeNY = dims[1];
	int iCubeNZ = dims[2];

	// С�����С
	double dCubeCX = spacing[0];
	double dCubeCY = spacing[1];
	double dCubeCZ = spacing[2];

	// �󷽿��С
	double BigCubeCX = dCubeCX * iCubeNX;
	double BigCubeCY = dCubeCY * iCubeNY;
	double BigCubeCZ = dCubeCZ * iCubeNZ;

	// 
	double x0 = origin[0];
	double y0 = origin[1];
	double z0 = origin[2];

	// С������8������Ҫ��ֵ
	double s[8];

	// С������8���������ꡢ�ݶ�
	double pts[8][3];

	// С������8�������ݶ�
	double gradients[8][3];

	// ��Ƭ�����ͳߴ�
	int sliceNums = dims[2];
	int sliceSize = dims[0] * dims[1];


	// ?
	int ptIds[3];	// ����id
	double xyz[3];	// ����
	double n[3];	// �ݶ�

	// ��
	int extent[6];
	//vtkInformation *inInfo = self->GetExecutive()->GetInputInformation(0, 0);
	//inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);

	// 
	KMarchingCubesTriangleCases* triCases = KMarchingCubesTriangleCases::GetCases();

	// ��ȸ��������Сֵ
	if (numValues < 1)
		return;

	double min = values[0];
	double max = values[0];
	for (int n = 1; n < numValues; n++)
	{
		if (values[n] < min)
		{
			min = values[n];
		}
		if (values[n] > max)
		{
			max = values[n];
		}
	}

	// �������е�����ϸ����ʹ��MC�㷨���������κ͵��ݶ�
	for (int kk = 0; kk < iCubeNZ - 1; kk++)
	{
		int k = kk;
		//int k = iCubeNZ - 1 - 1 - kk;

		// ��ƫ��
		int kOffset = k * sliceSize;

		// z����
		//pts[0][2] = origin[2] + (k+extent[4]) * spacing[2];
		pts[0][2] = origin[2] + k * spacing[2];
		double zp = pts[0][2] + spacing[2];

		for (int j = 0; j < iCubeNY; j++)
		{
			// ��ƫ��
			int jOffset = j * dims[0];

			// y����
			//pts[0][1] = origin[1] + (j + extent[2]) * spacing[1];
			pts[0][1] = origin[1] + j * spacing[1];
			double yp = pts[0][1] + spacing[1];

			for (int i = 0; i < iCubeNX; i++)
			{
				// ȡ��С�������8������ı���ֵ
				int idx = i + jOffset + kOffset;

				s[0] = scalars[idx];
				s[1] = scalars[idx + 1];
				s[2] = scalars[idx + dims[0] + 1];
				s[3] = scalars[idx + dims[0]];

				s[4] = scalars[idx + sliceSize];
				s[5] = scalars[idx + sliceSize + 1];
				s[6] = scalars[idx + sliceSize + dims[0] + 1];
				s[7] = scalars[idx + sliceSize + dims[0]];

				// Ҫ��ֵ���ڷ�Χ��
				if ((s[0] < min && s[1] < min && s[2] < min && s[3] < min &&
					s[4] < min && s[5] < min && s[6] < min && s[7] < min) ||
					(s[0] > max && s[1] > max && s[2] > max && s[3] > max &&
						s[4] > max && s[5] > max && s[6] > max && s[7] > max))
				{
					continue;
				}

				//create voxel points
				//pts[0][0] = origin[0] + (i + extent[0]) * spacing[0];
				pts[0][0] = origin[0] + i * spacing[0];
				double xp = pts[0][0] + spacing[0];

				pts[1][0] = xp;
				pts[1][1] = pts[0][1];
				pts[1][2] = pts[0][2];

				pts[2][0] = xp;
				pts[2][1] = yp;
				pts[2][2] = pts[0][2];

				pts[3][0] = pts[0][0];
				pts[3][1] = yp;
				pts[3][2] = pts[0][2];

				pts[4][0] = pts[0][0];
				pts[4][1] = pts[0][1];
				pts[4][2] = zp;

				pts[5][0] = xp;
				pts[5][1] = pts[0][1];
				pts[5][2] = zp;

				pts[6][0] = xp;
				pts[6][1] = yp;
				pts[6][2] = zp;

				pts[7][0] = pts[0][0];
				pts[7][1] = yp;
				pts[7][2] = zp;

				// �����ݶ� - gradients[8]
				bool needGradients = true;
				if (needGradients)
				{
					ComputePointGradient(i, j, k, sliceSize, scalars, dims, spacing, gradients[0]);
					ComputePointGradient(i + 1, j, k, sliceSize, scalars, dims, spacing, gradients[1]);
					ComputePointGradient(i + 1, j + 1, k, sliceSize, scalars, dims, spacing, gradients[2]);
					ComputePointGradient(i, j + 1, k, sliceSize, scalars, dims, spacing, gradients[3]);
					ComputePointGradient(i, j, k + 1, sliceSize, scalars, dims, spacing, gradients[4]);
					ComputePointGradient(i + 1, j, k + 1, sliceSize, scalars, dims, spacing, gradients[5]);
					ComputePointGradient(i + 1, j + 1, k + 1, sliceSize, scalars, dims, spacing, gradients[6]);
					ComputePointGradient(i, j + 1, k + 1, sliceSize, scalars, dims, spacing, gradients[7]);
				}

				// ��ֵ������
				double svalue = 50;	//����ֵ
				for (int contNum = 0; contNum < numValues; contNum++)
				{
					svalue = values[contNum];

					// ����8������������Ҫ��256���������һ��
					int index = 0;
					for (int ii = 0; ii < 8; ii++)
					{
						if (s[ii] >= svalue)
						{
							index |= CASE_MASK[ii];
						}
					}

					//������case�ǲ��������κ������Σ��ɴ�����
					if (index == 0 || index == 255) //no surface
					{
						continue;
					}

					KMarchingCubesTriangleCases* triCase = triCases + index;
					int* edge = triCase->Edges;

					for (; edge[0] > -1; edge += 3)
					{
						// insert triangle
						for (int ii = 0; ii < 3; ii++)
						{
							// ���Բ�ֵ�������� LERP
							int* vert = EDGES[edge[ii]];
							double t = (svalue - s[vert[0]]) / (s[vert[1]] - s[vert[0]]);

							// ��������
							double* p1 = pts[vert[0]];
							double* p2 = pts[vert[1]];
							xyz[0] = p1[0] + t * (p2[0] - p1[0]);
							xyz[1] = p1[1] + t * (p2[1] - p1[1]);
							xyz[2] = p1[2] + t * (p2[2] - p1[2]);

							// ���涥��
							{
								// �������ĵ�
								//double x = xyz[0];
								//double y = xyz[1];
								//double z = xyz[2];

								double x = xyz[0] - BigCubeCX / 2;
								double y = xyz[1] - BigCubeCY / 2;
								double z = xyz[2] - BigCubeCZ / 2;


								// ��׼������
								x /= BigCubeCX;
								y /= BigCubeCY;
								z /= BigCubeCZ;

								m_Vertices.push_back(Vector3(x, y, z));
								m_VertexValues.push_back(svalue);
							}

							// �����ݶ�
							double* n1 = gradients[vert[0]];
							double* n2 = gradients[vert[1]];
							n[0] = n1[0] + t * (n2[0] - n1[0]);
							n[1] = n1[1] + t * (n2[1] - n1[1]);
							n[2] = n1[2] + t * (n2[2] - n1[2]);

							// ��׼��
							KMaths::Normalize(n);

							// ����
							m_Normals.push_back(Vector3(n[0], n[1], n[2]));

						}//for (int ii = 0; ii < 3; ii++)

						//
						// check for degenerate triangle(����˻�������)
						//if (ptIds[0] != ptIds[1] &&
						//	ptIds[0] != ptIds[2] &&
						//	ptIds[1] != ptIds[2])
						//{
						//	newPolys->InsertNextCell(3, ptIds);
						//}


					}//for each triangle
				}//for all contours
			}//for i
		}// for j
	}// for k


	return;
}



//---------------------------------------------------------
//
//    z
//	  |
//	  |    y
//	  |   /
//	  |  /
//	  | /
//	  0--------------x
//
// �����壺8�����㡢6���桢12���ߡ�12�������Ρ�36����������
//
// scalars		- 3D�����Ҫ��ֵ
// dims[3]		- 3D����ά��(nx,ny,nz)
// cubeSize[3]	- �ƶ��������С(cx,cy,cz)
// cubeCount[3]	- �ƶ�����������
// values		- ��ֵ��ֵ
// numValues	- ��ֵ������
//
//---------------------------------------------------------
void KMarchingCubes::Building(double *scalars, int dims[3], double cubeSize[3], int cubeCount[3], double *values, int numValues)
{
	// CASE_MASK
	static int CASE_MASK[8] = { 1,2,4,8,16,32,64,128 };

	// ÿ����2������
	static int EDGES[12][2] =
	{
		{ 0,1 },{ 1,2 },{ 3,2 },{ 0,3 },
		{ 4,5 },{ 5,6 },{ 7,6 },{ 4,7 },
		{ 0,4 },{ 1,5 },{ 3,7 },{ 2,6 },
	};

	// ��ά���ά��
	int iSliceNX = dims[0];
	int iSliceNY = dims[1];
	int iSliceNZ = dims[2];

	// ��Ƭ�ߴ�
	int sliceSize = iSliceNX * iSliceNY;
	int allSize = iSliceNX * iSliceNY * iSliceNZ;

	// С��������
	int iCubeNX = cubeCount[0];
	int iCubeNY = cubeCount[1];
	int iCubeNZ = cubeCount[2];

	// С�����С
	double dCubeCX = cubeSize[0];
	double dCubeCY = cubeSize[1];
	double dCubeCZ = cubeSize[2];

	// �󷽿��С
	double BigCubeCX = dCubeCX * iCubeNX;
	double BigCubeCY = dCubeCY * iCubeNY;
	double BigCubeCZ = dCubeCZ * iCubeNZ;

	double x0 = 0;
	double y0 = 0;
	double z0 = 0;

	// �ƶ�������8���������ֵ
	double s[8];

	// �����ζ�������
	double xyz[3];

	// С������8����������
	double pts[8][3];

	// С������8�������ݶ�
	//double gradients[8][3];

	double min = values[0];
	double max = values[0];
	for (int n = 1; n < numValues; n++)
	{
		if (values[n] < min)
		{
			min = values[n];
		}
		if (values[n] > max)
		{
			max = values[n];
		}
	}

	// 
	KMarchingCubesTriangleCases* triCases = KMarchingCubesTriangleCases::GetCases();

	// �������е�����ϸ��ʹ��MC�㷨���������κ͵��ݶ�
	for (int k = 0; k < iCubeNZ; k++)
	{
		pts[0][2] = z0 + k * dCubeCZ;
		double zp = pts[0][2] + dCubeCZ;

		for (int j = 0; j < iCubeNY; j++)
		{
			pts[0][1] = y0 + j * dCubeCY;
			double yp = pts[0][1] + dCubeCY;

			for (int i = 0; i < iCubeNX; i++)
			{
				// С������8����������
				pts[0][0] = x0 + i * dCubeCX;
				double xp = pts[0][0] + dCubeCX;

				// p1 
				pts[1][0] = xp;
				pts[1][1] = pts[0][1];
				pts[1][2] = pts[0][2];

				// p2
				pts[2][0] = xp;
				pts[2][1] = yp;
				pts[2][2] = pts[0][2];

				// p3
				pts[3][0] = pts[0][0];
				pts[3][1] = yp;
				pts[3][2] = pts[0][2];

				// p4
				pts[4][0] = pts[0][0];
				pts[4][1] = pts[0][1];
				pts[4][2] = zp;

				// p5
				pts[5][0] = xp;
				pts[5][1] = pts[0][1];
				pts[5][2] = zp;

				// p6
				pts[6][0] = xp;
				pts[6][1] = yp;
				pts[6][2] = zp;

				// p7
				pts[7][0] = pts[0][0];
				pts[7][1] = yp;
				pts[7][2] = zp;


				double svalue = 50;	// ����ֵ
				for (int contNum = 0; contNum < numValues; contNum++)
				{
					svalue = values[contNum];

					// ����8������������Ҫ��256���������һ��
					int index = 0;
					int xindex, yindex, zindex;
					for (int vidx = 0; vidx < 8; vidx++)
					{
						//�����Ӧ�����������
						xindex = (int)(pts[vidx][0] / BigCubeCX * iSliceNX);
						if (xindex >= iSliceNX) xindex--;

						yindex = (int)(pts[vidx][1] / BigCubeCY * iSliceNY);
						if (yindex >= iSliceNY) yindex--;

						zindex = (int)(pts[vidx][2] / BigCubeCZ * iSliceNZ);
						if (zindex >= iSliceNZ) zindex--;

						int pos = zindex * iSliceNX*iSliceNY + yindex * iSliceNX + xindex;
						if (pos >= allSize)
							continue;

						// 8���������ֵ(�ȴ�����)
						s[vidx] = scalars[pos];

						if (s[vidx] >= svalue)
						{
							index |= 1 << vidx;
						}
					}

					//������case���������κ�������
					if (index == 0 || index == 255) //no surface
						continue;

					// Ҫ��ֵ���ڷ�Χ��
					bool allmin = s[0] < min && s[1] < min && s[2] < min && s[3] < min
						&& s[4] < min && s[5] < min && s[6] < min && s[7] < min;
					bool allmax = (s[0] > max && s[1] > max && s[2] > max && s[3] > max
						&& s[4] > max && s[5] > max && s[6] > max && s[7] > max);

					if (allmin || allmax)
						continue;

					// ����8�������ݶ�
					{

					}

					// 
					int* aedge = triCases[index].Edges;

					// for each triangle
					for (; aedge[0] > -1; aedge += 3)
					{
						//for one triangle
						for (int tri = 0; tri < 3; tri++)
						{
							// �ߵ������˵�
							int* vert = EDGES[aedge[tri]];
							if (KMaths::IsEqual(s[vert[1]], s[vert[0]]))
								break;

							// ���Բ�ֵ�������� LERP
							double t = (svalue - s[vert[0]]) / (s[vert[1]] - s[vert[0]]);

							double* pt1 = pts[vert[0]];
							double* pt2 = pts[vert[1]];

							xyz[0] = pt1[0] + t * (pt2[0] - pt1[0]);
							xyz[1] = pt1[1] + t * (pt2[1] - pt1[1]);
							xyz[2] = pt1[2] + t * (pt2[2] - pt1[2]);


							// �������ĵ�
							double x = xyz[0] - BigCubeCX / 2;
							double y = xyz[1] - BigCubeCY / 2;
							double z = xyz[2] - BigCubeCZ / 2;

							// ��׼������
							//x /= dBigCubeCX;
							//y /= dBigCubeCY;
							//z /= dBigCubeCZ;

							Vector3 vec = Vector3(x, z, y);
							m_Vertices.push_back(vec);
							m_VertexValues.push_back(svalue);

						}// for one triangle
					}//for each triangle
				}// for all contour
			}//for i
		}//for j
	}//for k

	return;
}




bool KMarchingCubes::IsUniquePoint(double xyz[3])
{
	return true;
}

// �����Ĳ�ַ�������ݶ�
// ��� n[3]
void KMarchingCubes::ComputePointGradient(int i, int j, int k, int sliceSize,
	double *scalars, int dims[3], double spacing[3], OUT double n[3])
{
	double *s = scalars;
	double sp, sm;

	// x-direction
	if (i == 0)
	{
		sp = s[i + 1 + j * dims[0] + k * sliceSize];
		sm = s[i + 0 + j * dims[0] + k * sliceSize];
		n[0] = (sm - sp) / spacing[0];
	}
	else if (i == (dims[0] - 1))
	{
		sp = s[i + j * dims[0] + k * sliceSize];
		sm = s[i - 1 + j * dims[0] + k * sliceSize];
		n[0] = (sm - sp) / spacing[0];
	}
	else
	{
		sp = s[i + 1 + j * dims[0] + k * sliceSize];
		sm = s[i - 1 + j * dims[0] + k * sliceSize];
		n[0] = 0.5 * (sm - sp) / spacing[0];
	}

	// y-direction
	if (j == 0)
	{
		sp = s[i + (j + 1)*dims[0] + k * sliceSize];
		sm = s[i + j * dims[0] + k * sliceSize];
		n[1] = (sm - sp) / spacing[1];
	}
	else if (j == (dims[1] - 1))
	{
		sp = s[i + j * dims[0] + k * sliceSize];
		sm = s[i + (j - 1)*dims[0] + k * sliceSize];
		n[1] = (sm - sp) / spacing[1];
	}
	else
	{
		sp = s[i + (j + 1)*dims[0] + k * sliceSize];
		sm = s[i + (j - 1)*dims[0] + k * sliceSize];
		n[1] = 0.5 * (sm - sp) / spacing[1];
	}

	// z-direction
	if (k == 0)
	{
		sp = s[i + j * dims[0] + (k + 1)*sliceSize];
		sm = s[i + j * dims[0] + k * sliceSize];
		n[2] = (sm - sp) / spacing[2];
	}
	else if (k == (dims[2] - 1))
	{
		sp = s[i + j * dims[0] + k * sliceSize];
		sm = s[i + j * dims[0] + (k - 1)*sliceSize];
		n[2] = (sm - sp) / spacing[2];
	}
	else
	{
		sp = s[i + j * dims[0] + (k + 1)*sliceSize];
		sm = s[i + j * dims[0] + (k - 1)*sliceSize];
		n[2] = 0.5 * (sm - sp) / spacing[2];
	}

	return;
}
