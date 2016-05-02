
#pragma once

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
};
