
#pragma once

class AnimationsDockable;

class DockView : public clan::View
{
public:
	DockView();

private:
	std::shared_ptr<AnimationsDockable> animations;
};
