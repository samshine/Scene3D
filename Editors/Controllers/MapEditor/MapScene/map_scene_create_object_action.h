
#pragma once

class MapSceneCreateObjectAction : public uicore::ViewAction
{
public:
	void pointer_press(uicore::PointerEvent &e) override;
	void pointer_release(uicore::PointerEvent &e) override;
	void pointer_move(uicore::PointerEvent &e) override;
};
