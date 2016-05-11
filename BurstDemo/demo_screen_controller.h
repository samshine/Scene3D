
#pragma once

class GPUCollision;

class DemoScreenController : public ScreenController
{
public:
	DemoScreenController();
	void update() override;

private:
	ScenePtr scene;
	SceneCameraPtr camera;
	SceneObjectPtr box;
	SceneLightPtr light1, light2;

	uicore::FontPtr font;

	uicore::ProgramObjectPtr program;
	uicore::StorageBufferPtr storage;
	uicore::StagingBufferPtr staging;

	struct Particle
	{
		uicore::Vec3f pos;
		float padding;
		uicore::Vec3f velocity;
		float life;
	};

	const int particle_count = 64;

	std::shared_ptr<GPUCollision> collision;
};
