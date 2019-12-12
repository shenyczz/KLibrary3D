#pragma once

#include "DXUtil.h"

using namespace dx12;
using namespace DirectX;

#define MaxLights 16

// 
// 灯光结构体
// 可以表示方向光源、点光源和聚光灯光源
//
struct Light
{
	XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };		// 光源颜色
	float FalloffStart = 1.0f;						// 点光源/聚光灯使用

	XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };		// directional/spot light only
	float FalloffEnd = 10.0f;						// 点光源/聚光灯使用

	XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };		// 点光源/聚光灯使用
	float SpotPower = 64.0f;						// 聚光灯使用
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

	// 名称
	_astring Name;

	// 材质常量缓冲区索引
	int MatCBIndex = -1;

	// 漫反射纹理的SRV堆索引
	int DiffuseSrvHeapIndex = -1;

	// 法线纹理的SRV堆索引
	int NormalSrvHeapIndex = -1;

	// 脏标记数量
	int Dirtys = 0;

	// 用于着色的材质常量缓冲区数据

	// DiffuseAlbedo - 漫反射反照率
	XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	// FresnelR0 - 材质属性
	XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	// Roughness - 粗糙度
	float Roughness = 0.25f;
	// MatTransform - 材质变换矩阵
	XMFLOAT4X4 MatTransform = DXUtils::Identity4x4;

	//@EndOf(Material)
};







