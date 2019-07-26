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
* bookBox.hlsl
* 
******************************************************************************/

//SamplerState g_sampler : register(s0, space0);
//SamplerState g_sampler : register(s0);
//Texture2D g_texture : register(t0);

// ����������(���Ʋ���Ҫ��Ҫ�ͳ����ṹһһ��Ӧ)
cbuffer cbbuf0 : register(b0)
{
	float4x4 gWorldViewProj;
};



// ���붥��(˳����Ҫ����Ҫ�����������ƺ�����)
struct VertexIn
{
	float3 PosL  : POSITION;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
	float2 uv : TEXCOORD;
};

// �������
struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
};

// ������ɫ��
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

// ������ɫ��
float4 PS(VertexOut pin) : SV_TARGET
{
	float4 clr = float4(1.0f, 0.0f, 0.0f, 1.0f);

	clr = pin.Color;

	return clr;
}

