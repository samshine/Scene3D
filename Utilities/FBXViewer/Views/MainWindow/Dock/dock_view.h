
#pragma once

#include <UICore/View/view.h>
#include <UICore/StandardViews/button_view.h>

class AnimationsDockable;

class DockView : public uicore::View
{
public:
	DockView();

private:
	std::shared_ptr<AnimationsDockable> animations;
};
