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
* bookShapes.hlsl
* 
******************************************************************************/

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
};

cbuffer cbPass : register(b1)
{
	float4x4	gView;
	float4x4	gInvView;
	float4x4	gProj;
	float4x4	gInvProj;
	float4x4	gViewProj;
	float4x4	gInvViewProj;

	float3		gEyePosW;

	float		cbPerObjectPad1;

	float2		gRenderTargetSize;
	float2		gInvRenderTargetSize;

	float		gNearZ;
	float		gFarZ;

	float		gTotalTime;
	float		gDeltaTime;
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
		//float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
		//vout.PosH = mul(posW, gViewProj);
		//vout.Color = vin.Color;

		float4 pos_h = float4(vin.PosL, 1.0f);
		float4 pos_w = mul(pos_h, gWorld);

		vout.PosH = mul(pos_w, gViewProj);
		vout.Color = vin.Color;
	}

	return vout;
}
/*

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosH = mul(posW, gViewProj);
    vout.Color = vin.Color;
    
    return vout;
}

*/

// 像素着色器
float4 PS(VertexOut pin) : SV_TARGET
{
	float4 clr = float4(1.0f, 0.0f, 0.0f, 1.0f);

	clr = pin.Color;

	return clr;
}

