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
* bookBox2.hlsl
* 
******************************************************************************/

// ����������(���Ʋ���Ҫ��Ҫ�ͳ����ṹһһ��Ӧ)
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
};

cbuffer cbPass : register(b1)
{
	float4x4	gView;
	float4x4	gProj;
	float4x4	gViewProj;
	
}


// ���붥��(˳����Ҫ����Ҫ�����������ƺ�����)
struct VertexIn
{
	float3 PosL  : POSITION;
	float4 Color : COLOR;
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
		float4x4 world = gWorld;
		float4x4 view = gView;
		float4x4 proj = gProj;
		float4x4 viewProj = gViewProj;

		float4x4 worldViewProj = mul(world, viewProj);

		
		float4 pos_h = float4(vin.PosL, 1.0f);

		vout.PosH = mul(pos_h, worldViewProj);
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

