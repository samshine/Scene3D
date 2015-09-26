
#pragma once

#include <memory>
#include <string>

class SceneCacheImpl;

class SceneCache
{
public:
	SceneCache();
	SceneCache(uicore::GraphicContext &gc, const std::string &shader_path);
	bool is_null() const { return !impl; }

private:
	std::shared_ptr<SceneCacheImpl> impl;

	friend class Scene_Impl;
};
