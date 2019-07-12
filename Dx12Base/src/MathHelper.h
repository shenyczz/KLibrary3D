/******************************************************************************
*
* Announce: KLibrary, used to achieve data visualization.
*           Copyright (C) ShenYongchen.
*           All rights reserved.
*   Author: ÉêÓÀ³½.Ö£ÖÝ (shenyczz@163.com)
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

namespace DX12
{

	class DX12BASE_EXPORTS_CLASS MathHelper
	{
	private:
		MathHelper() = delete;
		~MathHelper() = default;

	public:

		// Çò×ø±ê×ª»»Î»µÑ¿¨¶û×ø±ê
		// theta	- 
		// phi		- 
		static XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
		{
			return XMVectorSet(
				radius*sinf(phi)*cosf(theta),
				radius*cosf(phi),
				radius*sinf(phi)*sinf(theta),
				1.0f);
		}

		// ·´×ªÖÃ
		// typedef const XMMATRIX& CXMMATRIX
		static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
		{
			// Inverse-transpose is just applied to normals.  So zero out 
			// translation row so that it doesn't get into our inverse-transpose
			// calculation--we don't want the inverse-transpose of the translation.
			DirectX::XMMATRIX A = M;
			A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
			return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
		}

		// 4x4µ¥Î»Õó
		static DirectX::XMFLOAT4X4 Identity4x4()
		{
			static DirectX::XMFLOAT4X4 I(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

			return I;
		}

		// Returns the polar angle of the point (x,y) in [0, 2*PI).
		static float AngleFromXY(float x, float y);

		// ?
		static DirectX::XMVECTOR RandUnitVec3();

		// ?
		// Hemisphere£¨°ëÇò£©
		static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);


		//@EndOf(MathHelper)
	};


}//DX12
