
#pragma once

class GPUCollision;

class RaytracerScreenController : public ScreenController
{
public:
	RaytracerScreenController();
	void update() override;

private:
	uicore::FontPtr font;
	std::shared_ptr<GPUCollision> collision;
};
