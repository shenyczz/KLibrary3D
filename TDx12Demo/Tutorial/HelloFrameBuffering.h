#pragma once

class HelloFrameBuffering : public Dx12Sample
{
public:
	HelloFrameBuffering(KWindow* pWindow);
	~HelloFrameBuffering();

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

protected:
	virtual void InitializeAssets() override;
	virtual void PopulateCommandList() override;


};

