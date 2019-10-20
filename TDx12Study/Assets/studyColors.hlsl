//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************


//---------------------------------------------------------寄存器
SamplerState g_sampler : register(s0);		// 本例使用静态采样器
Texture2D g_texture : register(t0, space0);


cbuffer _SceneConstant : register(b0)
{
	float4 offset;
};
//---------------------------------------------------------

//---------------------------------------------------------结构
struct VertexIn
{
	float3 PosL  : POSITION;
	float4 Color : COLOR;
};

struct TexVertexIn
{
	float3 PosL  : POSITION;
	float2 uv : TEXCOORD;
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
	float2 uv : TEXCOORD;
};
//---------------------------------------------------------

//---------------------------------------------------------函数
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = float4(vin.PosL, 1.0f);
	vout.Color = vin.Color;

	return vout;
}

// 常量
VertexOut VS_CB(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = float4(vin.PosL, 1.0f);
	vout.PosH.x += offset.x;
	vout.Color = vin.Color;

	return vout;
}

// 纹理
VertexOut VS_Tex(TexVertexIn vin)
{
	VertexOut vout;

	vout.PosH = float4(vin.PosL, 1.0f);
	vout.uv = vin.uv;
	vout.Color = vin.Color;

	return vout;
}
//---------------------------------------------------------


//---------------------------------------------------------
float4 PS(VertexOut pin) : SV_TARGET
{
	return pin.Color;
}
float4 PS_Tex(VertexOut pin) : SV_TARGET
{
	pin.Color = g_texture.Sample(g_sampler, pin.uv);
	return pin.Color;
}
//---------------------------------------------------------

/*

struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float4 uv : TEXCOORD)
{
	PSInput result;

	result.position = position;
	result.uv = uv;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return g_texture.Sample(g_sampler, input.uv);
}

*/
