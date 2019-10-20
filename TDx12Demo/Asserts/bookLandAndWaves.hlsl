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
* bookLandAndWaves.hlsl
* 
******************************************************************************/


// 常量缓冲区(名称不重要，要和常量结构对应)
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld; 
};

// 常量缓冲区(名称不重要，要和常量结构对应)
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


// 输入顶点(顺序不重要，重要的是语义名称和索引)
struct VertexIn
{
	float3 PosL		: POSITION;
	float3 Normal	: NORMAL;
	float3 TangeU	: NORMAL;
	float2 Tex		: TEXCOORD;
	float4 Color	: COLOR;
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
	
	// Transform to homogeneous clip space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    vout.PosH = mul(posW, gViewProj);
    vout.Color = vin.Color;
    
    return vout;

}

// 像素着色器
float4 PS(VertexOut pin) : SV_TARGET
{
    return pin.Color;
}

