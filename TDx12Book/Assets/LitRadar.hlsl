/******************************************************************************
 *
 * Announce: HLSL, used to achieve data visualization on DirectX 12.
 *           Copyright (C) ShenYongchen.
 *           All rights reserved.
 *   Author: 申永辰.郑州 (shenyczz@163.com)
 *
******************************************************************************/


#define NUM_DIR_LIGHTS 6	// 方向光源
#define NUM_POINT_LIGHTS 0	// 点光源
#define NUM_SPOT_LIGHTS 0	// 聚光灯光源

#include "LitRadarUtil.hlsli" 

// Constant data that varies per frame.

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};

cbuffer cbMaterial : register(b1)
{
	float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float  gRoughness;
	float4x4 gMatTransform;
};

// Constant data that varies per material.
cbuffer cbPass : register(b2)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;

    float3 gEyePosW;
    float cbPerObjectPad1;

    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;

    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;

    float4 gAmbientLight;

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
	Light gLights[MaxLights];
};
 
struct VertexIn
{
	float3 PosL		: POSITION;
    float3 NormalL	: NORMAL;
	float4 Color	: COLOR;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
    float3 PosW		: POSITION;
    float3 NormalW	: NORMAL;
	float4 Color	: COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	// 颜色
	vout.Color = vin.Color;
	
	// 转换到世界空间
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

	// 假设不均匀缩放;否则,需要使用世界矩阵的逆转置
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	// 变换到均匀剪辑空间
    vout.PosH = mul(posW, gViewProj);

    return vout;
}


float4 PS_0(VertexOut pin)
{
    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

	float4 ambientLight = gAmbientLight;	// 环境灯光
	float4 diffuseAlbedo = pin.Color;	// 漫反射反照率
	float3 fresnelR0 = gFresnelR0;			// 
	float shininess = 1.0f - gRoughness;

	// Indirect lighting.
	// 计算间接光 = 环境光 * 漫反射
	float4 ambient = ambientLight * diffuseAlbedo;

	// 计算灯光
	float3 shadowFactor = 1.0f;
	Material mat = { diffuseAlbedo, fresnelR0, shininess };
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

	// 灯光颜色
	float4 litColor = ambient + directLight;
	litColor.a = gDiffuseAlbedo.a;

	return litColor;
}

float4 PS_transparent(VertexOut pin)
{
    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

	float4 ambientLight = gAmbientLight;	// 环境灯光
	float4 diffuseAlbedo = gDiffuseAlbedo;	// 漫反射反照率
	float3 fresnelR0 = gFresnelR0;			// 
	float shininess = 1.0f - gRoughness;

	// 计算间接光 = 环境光 * 漫反射
	float4 ambient = ambientLight * diffuseAlbedo;

	// 计算灯光
	float3 shadowFactor = 1.0f;
	Material mat = { diffuseAlbedo, fresnelR0, shininess };
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

	// 灯光颜色
	float4 litColor = ambient + directLight;
	litColor.a = gDiffuseAlbedo.a;

	return litColor;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 ps_color = 0;

	//ps_color = PS_0(pin);
	ps_color = PS_transparent(pin);

	return ps_color;
}
