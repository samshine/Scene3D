
#pragma once

class SceneView;

class SceneController : public clan::ViewController
{
public:
	SceneController();

private:
	std::shared_ptr<SceneView> scene_view();

	clan::SlotContainer slots;
};
