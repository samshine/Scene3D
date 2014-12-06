
#pragma once

class SceneView;

class SceneController : public clan::ViewController
{
public:
	SceneController();

private:
	std::shared_ptr<SceneView> scene_view();
	void update_anim();

	clan::SlotContainer slots;

	bool left_down = false;
	bool up_down = false;
};
