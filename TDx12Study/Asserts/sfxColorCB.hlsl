/******************************************************************************
*
* Announce: Copyright (c) Shenyc. All rights reserved.
*   Author: 申永辰.郑州 (shenyczz@163.com)
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
******************************************************************************/


// 常量缓冲区(名称)
cbuffer cbuf : register(b0)
{
	float4 offset;
};

// 输入顶点
struct VertexIn
{
	float3 PosL  : POSITION;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float2 uv : TEXCOORD;
};

// 输出顶点
struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
};

// 顶点着色器
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	{
		vout.PosH = float4(vin.PosL, 1.0f);
		vout.PosH += offset;
		vout.Color = vin.Color;
	}

	return vout;
}

// 像素着色器
float4 PS(VertexOut pin) : SV_TARGET
{
	float4 clr = float4(1.0f, 0.0f, 0.0f, 1.0f);

	clr = pin.Color;
	//clr = float4(0.9f, 0.9f, 0.0f, 1.0f);

	return clr;
}

