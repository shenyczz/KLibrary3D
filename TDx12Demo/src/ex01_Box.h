#pragma once
#include "Mouse.h"

class ex01_Box : public dx12Sample
{
public:
	ex01_Box();
	~ex01_Box();

protected:
	// ���غ�������
	void BuildAssets() override;
	void BuildRootSignature();

	void Resized() override;
	void Update() override;
	void Draw() override;
	void Shutdown() override { dx12Sample::Shutdown(); }
	void DoAppIdle() override { dx12Sample::DoAppIdle(); }

private:
	ComPtr<ID3D12RootSignature> m_RootSignature;














	std::unique_ptr<DirectX::Mouse> m_pMouse;						// ���
	DirectX::Mouse::ButtonStateTracker m_MouseTracker;				// ���״̬׷����




	//@EndOf(Adx12Demo)
};

