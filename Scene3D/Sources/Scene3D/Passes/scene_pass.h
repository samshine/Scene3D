
#pragma once

class Scene_Impl;

class ScenePass
{
public:
	virtual ~ScenePass() { }

	virtual std::string name() const = 0;
	virtual void run(const uicore::GraphicContextPtr &gc, Scene_Impl *scene) = 0;
	virtual void update(const uicore::GraphicContextPtr &gc, float time_elapsed) { }
};
