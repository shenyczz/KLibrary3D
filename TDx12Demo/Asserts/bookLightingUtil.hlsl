//***************************************************************************************
// LightingUtil.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Contains API for shader lighting.
//***************************************************************************************

#define MaxLights 16 

struct Light
{
    float3 Strength;
    float FalloffStart; // point/spot light only
    float3 Direction;   // directional/spot light only
    float FalloffEnd;   // point/spot light only
    float3 Position;    // point light only
    float SpotPower;    // spot light only
};

struct Material
{
    float4 DiffuseAlbedo;	// �����䷴����
    float3 FresnelR0;		// ����������
    float Shininess;		// �����
};

// ˥������
// Ӧ������Դ��۹�ƹ�Դ
float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Linear falloff.
    return saturate((falloffEnd-d) / (falloffEnd - falloffStart));
}

// Schlick gives an approximation to Fresnel reflectance (see pg. 233 "Real-Time Rendering 3rd Ed.").
// R0 = ( (n-1)/(n+1) )^2, where n is the index of refraction.
// ������������̵�ʯ��˽���
// ���ڹ�����L����淨��n֮��ļнǣ����ݷ�����ЧӦ���Ƽ����
// ��nΪ���ߵı����������İٷֱ�
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));

    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0)*(f0*f0*f0*f0*f0);

    return reflectPercent;
}

// ���㷴�䵽�۲������еĹ�������ֵΪ����������뾵�淴��������ܺ�
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
	// m�ɹ�����Ƶ�������������ɴֲڶ����
    const float m = mat.Shininess * 256.0f;
    float3 halfVec = normalize(toEye + lightVec);

    float roughnessFactor = (m + 8.0f)*pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);

    float3 specAlbedo = fresnelFactor*roughnessFactor;

    // Our spec formula goes outside [0,1] range, but we are 
    // doing LDR rendering.  So scale it down a bit.
	// �������ǽ��е���LDR(Low Dynamic Rang,�Ͷ�̬��Χ)����Ⱦ��
	// ��spec(���淴��)��ʽ�õ��Ľ����Ȼ�ᳬ��[0,1]������ڴ�
	// ���䰴������СһЩ
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for directional lights.
// �����Դ
//---------------------------------------------------------------------------------------
float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
    // The light vector aims opposite the direction the light rays travel.
	// ����������ߴ����ķ��������෴
    float3 lightVec = -L.Direction;

    // Scale light down by Lambert's cosine law.
	// ͨ���ʲ����Ҷ��ɰ��������͹�ǿ
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for point lights.
// ���Դ
//---------------------------------------------------------------------------------------
float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // The vector from the surface to the light.
	// �Ա���ָ���Դ������
    float3 lightVec = L.Position - pos;

    // The distance from surface to light.
	// ���浽��Դ�ľ���
    float d = length(lightVec);

    // ��Χ���
    if(d > L.FalloffEnd)
        return 0.0f;

	// �淶��������
    lightVec /= d;

    // Scale light down by Lambert's cosine law.
	// ͨ���ʲ����Ҷ��ɰ��������͹�ǿ
	float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    // Attenuate light by distance.
	// ���ݾ��������˥��
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for spot lights.
// �۹�ƹ�Դ
//---------------------------------------------------------------------------------------
float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // The vector from the surface to the light.
	// �Ա���ָ���Դ������
	float3 lightVec = L.Position - pos;

    // The distance from surface to light.
	// ���浽��Դ�ľ���
	float d = length(lightVec);

	// ��Χ���
	if(d > L.FalloffEnd)
        return 0.0f;

	// �淶��������
	lightVec /= d;

	// ͨ���ʲ����Ҷ��ɰ��������͹�ǿ
	float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

	// ���ݾ��������˥��
	float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    // Scale by spotlight
	// ���ݾ۹������ģ�ͶԹ�ǿ�������Ŵ���
    float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
    lightStrength *= spotFactor;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

// 
float4 ComputeLighting(Light gLights[MaxLights], Material mat,
                       float3 pos, float3 normal, float3 toEye,
                       float3 shadowFactor)
{
    float3 result = 0.0f;

    int i = 0;

#if (NUM_DIR_LIGHTS > 0)
    for(i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], mat, normal, toEye);
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

