//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

// 常量缓冲区
cbuffer SceneConstantBuffer : register(b0)
{
	float4 offset;	// 偏移量
};

struct PSInput
{
	float4 position : SV_POSITION;	// SV_ 标识系统顶点?
	float4 color : COLOR;
};


// 顶点着色
PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
	PSInput result;

	result.position = position + offset;
	result.color = color;

	return result;
}

// 像素着色
float4 PSMain(PSInput input) : SV_TARGET
{
	return input.color;
}
