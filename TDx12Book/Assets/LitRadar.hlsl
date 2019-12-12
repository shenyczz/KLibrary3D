/******************************************************************************
 *
 * Announce: HLSL, used to achieve data visualization on DirectX 12.
 *           Copyright (C) ShenYongchen.
 *           All rights reserved.
 *   Author: ������.֣�� (shenyczz@163.com)
 *
******************************************************************************/


#define NUM_DIR_LIGHTS 6	// �����Դ
#define NUM_POINT_LIGHTS 0	// ���Դ
#define NUM_SPOT_LIGHTS 0	// �۹�ƹ�Դ

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

	// ��ɫ
	vout.Color = vin.Color;
	
	// ת��������ռ�
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

	// ���費��������;����,��Ҫʹ������������ת��
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	// �任�����ȼ����ռ�
    vout.PosH = mul(posW, gViewProj);

    return vout;
}


float4 PS_0(VertexOut pin)
{
    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

	float4 ambientLight = gAmbientLight;	// �����ƹ�
	float4 diffuseAlbedo = pin.Color;	// �����䷴����
	float3 fresnelR0 = gFresnelR0;			// 
	float shininess = 1.0f - gRoughness;

	// Indirect lighting.
	// �����ӹ� = ������ * ������
	float4 ambient = ambientLight * diffuseAlbedo;

	// ����ƹ�
	float3 shadowFactor = 1.0f;
	Material mat = { diffuseAlbedo, fresnelR0, shininess };
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

	// �ƹ���ɫ
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

	float4 ambientLight = gAmbientLight;	// �����ƹ�
	float4 diffuseAlbedo = gDiffuseAlbedo;	// �����䷴����
	float3 fresnelR0 = gFresnelR0;			// 
	float shininess = 1.0f - gRoughness;

	// �����ӹ� = ������ * ������
	float4 ambient = ambientLight * diffuseAlbedo;

	// ����ƹ�
	float3 shadowFactor = 1.0f;
	Material mat = { diffuseAlbedo, fresnelR0, shininess };
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

	// �ƹ���ɫ
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
