#pragma once

class Adx12Demo : public dx12Sample
{
public:
	Adx12Demo();
	~Adx12Demo();

protected:
	// жидиКЏЪ§бљР§
	void BuildAssets() override;
	void Resized() override;
	void Update() override;
	void Draw() override;
	void Shutdown() override { dx12Sample::Shutdown(); }
	void DoAppIdle() override { dx12Sample::DoAppIdle(); }

	//@EndOf(Adx12Demo)
};
