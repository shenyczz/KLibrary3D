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
* bookLandAndWaves.hlsl
* 
******************************************************************************/


// ����������(���Ʋ���Ҫ��Ҫ�ͳ����ṹ��Ӧ)
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld; 
};

// ����������(���Ʋ���Ҫ��Ҫ�ͳ����ṹ��Ӧ)
cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;

    //float4x4 gInvView;
    //float4x4 gInvProj;
    //float4x4 gInvViewProj;

    //float3 gEyePosW;

    //float cbPerObjectPad1;

    //float2 gRenderTargetSize;
    //float2 gInvRenderTargetSize;

    //float gNearZ;
    //float gFarZ;

    //float gTotalTime;
    //float gDeltaTime;
};


// ���붥��(˳����Ҫ����Ҫ�����������ƺ�����)
struct VertexIn
{
	float3 PosL		: POSITION;
	float3 Normal	: NORMAL;
	float3 TangeU	: NORMAL;
	float2 Tex		: TEXCOORD;
	float4 Color	: COLOR;
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
	
	// Transform to homogeneous clip space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    vout.PosH = mul(posW, gViewProj);
    vout.Color = vin.Color;
    
    return vout;

}

// ������ɫ��
float4 PS(VertexOut pin) : SV_TARGET
{
    return pin.Color;
}

