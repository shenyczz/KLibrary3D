#pragma once


class Abook11Demo : public dx11Sample
{
public:
	Abook11Demo();
	~Abook11Demo();

private:
	void BuildAssets() override;
	void Resized() override;
	void Update() override;
	void Draw() override;
	void Shutdown() override;


	//@EndOf(Abook11Demo)
};

