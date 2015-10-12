
#pragma once

#include <memory>
#include <string>

class SceneCache
{
public:
	static std::shared_ptr<SceneCache> create(const uicore::GraphicContextPtr &gc, const std::string &shader_path);

	virtual ~SceneCache() { }
};

typedef std::shared_ptr<SceneCache> SceneCachePtr;
