#pragma once

// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().

class HelloWindow : public Dx12Sample
{
public:
	HelloWindow(KWindow* pWindow) : Dx12Sample(pWindow) {}
	~HelloWindow() {}

protected:
	virtual void InitializeAssets() override;
	virtual void PopulateCommandList() override;

};

