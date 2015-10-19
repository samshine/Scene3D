
#pragma once

class SceneImpl;

class ScenePass
{
public:
	virtual ~ScenePass() { }

	virtual std::string name() const = 0;
	virtual void run(const uicore::GraphicContextPtr &gc, SceneImpl *scene) = 0;
	virtual void update(const uicore::GraphicContextPtr &gc, float time_elapsed) { }
};
