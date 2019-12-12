#pragma once

class Adx11Demo : public dx11Sample
{
public:
	Adx11Demo();
	~Adx11Demo();

private:
	// жидиКЏЪ§бљР§
	void BuildAssets() override;
	void Resized() override;
	void Update() override;
	void Draw() override;
	void Shutdown() override { dx11Sample::Shutdown(); }
	void DoAppIdle() override { dx11Sample::DoAppIdle(); }


	//@EndOf(Adx11Demo)
};

