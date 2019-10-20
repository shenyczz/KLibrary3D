/******************************************************************************
 *
 * Announce: HLSL, used to achieve data visualization on DirectX 12.
 *           Copyright (C) ShenYongchen.
 *           All rights reserved.
 *   Author: ������.֣�� (shenyczz@163.com)
 *
 *  Explain: Structures and Functions for lighting.
 *
******************************************************************************/

#define MaxLights 16 

// �ƹ� 
struct Light
{
    float3 Strength;	// ǿ��
    float FalloffStart; // (���ڵ��Դ���۹��)

    float3 Direction;   // ����(���ھ۹��)
    float FalloffEnd;   // (���ڵ��Դ���۹��)

    float3 Position;    // (���ڵ��Դ)
    float SpotPower;    // (���ھ۹��)
};

// ����
struct Material
{
    float4 DiffuseAlbedo;	// ����ⷴ����

    float3 FresnelR0;		// ����������R0����
    float Shininess;		// �⻬��
};

//---------------------------------------------------------------------------------------
// ����˥��
//---------------------------------------------------------------------------------------
float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // ���Բ�ֵ
    return saturate((falloffEnd-d) / (falloffEnd - falloffStart));
}

//---------------------------------------------------------------------------------------
// Schlick gives an approximation to Fresnel reflectance (see pg. 233 "Real-Time Rendering 3rd Ed.").
// R0 = [(n-1)/(n+1)]^2, where n is the index of refraction(n�Ƿ�����ָ��).
//---------------------------------------------------------------------------------------
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
	// ���������
    float cosIncidentAngle = saturate(dot(normal, lightVec));

    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0)*(f0*f0*f0*f0*f0);

    return reflectPercent;
}

//---------------------------------------------------------------------------------------
// ���㣿��
// lightStrength	- �ƹ�ǿ��
// lightVec			- �ƹ�����
// normal			- ����
// toEye			- ������
// mat				- ����
//---------------------------------------------------------------------------------------
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
    const float m = mat.Shininess * 256.0f;
    float3 halfVec = normalize(toEye + lightVec);

    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);

    float3 specAlbedo = fresnelFactor * roughnessFactor;

    // Our spec formula goes outside [0,1] range, but we are 
    // doing LDR rendering.  So scale it down a bit.
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

//---------------------------------------------------------------------------------------
// ƽ�й�Դ���շ���
//---------------------------------------------------------------------------------------
float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
    // The light vector aims opposite the direction the light rays travel.
    float3 lightVec = -L.Direction;

    // Scale light down by Lambert's cosine law.
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//---------------------------------------------------------------------------------------
// ���Դ���շ���
//---------------------------------------------------------------------------------------
float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // The vector from the surface to the light.
    float3 lightVec = L.Position - pos;

    // The distance from surface to light.
    float d = length(lightVec);

    // Range test.
    if(d > L.FalloffEnd)
        return 0.0f;

    // Normalize the light vector.
    lightVec /= d;

    // Scale light down by Lambert's cosine law�������������ɣ�.
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    // Attenuate��˥���� light by distance.
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//---------------------------------------------------------------------------------------
// �۹�ƹ��շ���
//---------------------------------------------------------------------------------------
float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // The vector from the surface to the light.
    float3 lightVec = L.Position - pos;

    // The distance from surface to light.
    float d = length(lightVec);

    // Range test.
    if(d > L.FalloffEnd)
        return 0.0f;

    // Normalize the light vector.
    lightVec /= d;

    // Scale light down by Lambert's cosine law.
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    // Attenuate light by distance.
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    // Scale by spotlight
    float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
    lightStrength *= spotFactor;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//---------------------------------------------------------------------------------------
// �����Դ
//---------------------------------------------------------------------------------------
float4 ComputeLighting(Light gLights[MaxLights],
	Material mat,
	float3 pos,
	float3 normal,
	float3 toEye,
	float3 shadowFactor)
{
    float3 result = 0.0f;

    int i = 0;

#if (NUM_DIR_LIGHTS > 0)
    for(i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
		//result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], mat, normal, toEye);
		result += ComputeDirectionalLight(gLights[i], mat, normal, toEye);
	}
#endif

#if (NUM_POINT_LIGHTS > 0)
    for(i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS+NUM_POINT_LIGHTS; ++i)
    {
        result += ComputePointLight(gLights[i], mat, pos, normal, toEye);
    }
#endif

#if (NUM_SPOT_LIGHTS > 0)
    for(i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        result += ComputeSpotLight(gLights[i], mat, pos, normal, toEye);
    }
#endif 

    return float4(result, 0.0f);
}


