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
* bookBox.hlsl
* 
******************************************************************************/

//SamplerState g_sampler : register(s0, space0);
//SamplerState g_sampler : register(s0);
//Texture2D g_texture : register(t0);

// 常量缓冲区(名称不重要，要和常量结构一一对应)
cbuffer cbbuf0 : register(b0)
{
	float4x4 gWorldViewProj;
};



// 输入顶点(顺序不重要，重要的是语义名称和索引)
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
		float4 pos_h = float4(vin.PosL, 1.0f);

		// Transform to homogeneous clip space.
		vout.PosH = mul(pos_h, gWorldViewProj);

		vout.Color = vin.Color;
	}

	return vout;
}

// 像素着色器
float4 PS(VertexOut pin) : SV_TARGET
{
	float4 clr = float4(1.0f, 0.0f, 0.0f, 1.0f);

	clr = pin.Color;

	return clr;
}

