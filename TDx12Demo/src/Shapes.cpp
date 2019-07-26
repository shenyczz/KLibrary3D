#include "stdafx.h"
#include "Shapes.h"


Shapes::Shapes()
{
	m_Theta = XM_PI * (3.0f / 2);
	m_Phi = XM_PI / 2;
	m_Radius = 15.0f;
	//float mTheta = 1.5f*XM_PI;
	//float mPhi = 0.2f*XM_PI;
	//float mRadius = 15.0f;
}

Shapes::~Shapes()
{
}


void Shapes::OnResize()
{
	Dx12Book::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH((float)KMaths::PI / 4, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);
}

void Shapes::OnUpdate()
{
}

void Shapes::BuildRootSignature() {}

void Shapes::BuildShadersAndInputLayout() {}

void Shapes::BuildPipelineStateObject() {}

void Shapes::BuildMeshData() {}

void Shapes::BuildConstantBufferAndView() {}

void Shapes::ReccorCommand() {}





void Shapes::UpdateCamera()
{
	KVector3 v = KMaths::SphericalToCartesian(m_Radius, m_Theta, m_Phi);
	m_EyePos.x = v.x;

	// Convert Spherical to Cartesian coordinates.
	//m_EyePos.x = m_Radius * sinf(m_Phi)*cosf(m_Theta);
	//m_EyePos.z = m_Radius * sinf(m_Phi)*sinf(m_Theta);
	//m_EyePos.y = m_Radius * cosf(m_Phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(m_EyePos.x, m_EyePos.y, m_EyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_View, view);
}



//mEyePos.x = mRadius * sinf(mPhi)*cosf(mTheta);
//mEyePos.z = mRadius * sinf(mPhi)*sinf(mTheta);
//mEyePos.y = mRadius * cosf(mPhi);
//
//// Build the view matrix.
//XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
//XMVECTOR target = XMVectorZero();
//XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//
//XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
//XMStoreFloat4x4(&mView, view);
