/******************************************************************************
*
* Announce: Copyright (c) Shenyc. All rights reserved.
*   Author: ������.֣�� (shenyczz@163.com)
*
* THIS CODE IS LICENSED UNDER THE MIT LICENSE (MIT).
* THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
* IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
* PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
*
******************************************************************************/

//SamplerState g_sampler : register(s0, space0);
SamplerState g_sampler : register(s0);
Texture2D g_texture : register(t0);


// ���붥��
struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Color  : COLOR;
	float2 uv : TEXCOORD;
};

// �������
struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 uv : TEXCOORD;
};

// ������ɫ��
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	{
		vout.PosH = float4(vin.PosL, 1.0f);
		vout.uv = vin.uv;
	}

	return vout;
}

// ������ɫ��
float4 PS(VertexOut pin) : SV_TARGET
{
	float4 clr = float4(1,0,0,0);
	//clr = pin.Color;
	//return clr;
	return g_texture.Sample(g_sampler, pin.uv);
}
