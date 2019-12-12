#pragma once

#include "DXUtil.h"

using namespace dx12;
using namespace DirectX;

#define MaxLights 16

// 
// �ƹ�ṹ��
// ���Ա�ʾ�����Դ�����Դ�;۹�ƹ�Դ
//
struct Light
{
	XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };		// ��Դ��ɫ
	float FalloffStart = 1.0f;						// ���Դ/�۹��ʹ��

	XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };		// directional/spot light only
	float FalloffEnd = 10.0f;						// ���Դ/�۹��ʹ��

	XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };		// ���Դ/�۹��ʹ��
	float SpotPower = 64.0f;						// �۹��ʹ��
};


struct Texture
{
	_astring Name;
	_tstring Filename;
	ComPtr<ID3D12Resource> Resource = nullptr;
	ComPtr<ID3D12Resource> UploadHeap = nullptr;
};



// Simple struct to represent a material for our demos.  A production 3D engine
// would likely create a class hierarchy of Materials.
struct Material
{
	Material(int dirtys)
		: Dirtys(dirtys) {}

	// ����
	_astring Name;

	// ���ʳ�������������
	int MatCBIndex = -1;

	// �����������SRV������
	int DiffuseSrvHeapIndex = -1;

	// ���������SRV������
	int NormalSrvHeapIndex = -1;

	// ��������
	int Dirtys = 0;

	// ������ɫ�Ĳ��ʳ�������������

	// DiffuseAlbedo - �����䷴����
	XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	// FresnelR0 - ��������
	XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	// Roughness - �ֲڶ�
	float Roughness = 0.25f;
	// MatTransform - ���ʱ任����
	XMFLOAT4X4 MatTransform = DXUtils::Identity4x4;

	//@EndOf(Material)
};







