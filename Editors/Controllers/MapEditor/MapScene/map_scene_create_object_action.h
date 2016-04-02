
#pragma once

class MapSceneController;

class MapSceneCreateObjectAction : public uicore::ViewAction
{
public:
	MapSceneCreateObjectAction(MapSceneController *controller, int view_index);

	void pointer_press(uicore::PointerEvent *e) override;
	void pointer_release(uicore::PointerEvent *e) override;
	void pointer_move(uicore::PointerEvent *e) override;

private:
	MapSceneController *controller;
	int view_index = 0;
};
