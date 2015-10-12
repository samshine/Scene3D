
#include "precomp.h"
#include "Scene3D/scene_cache.h"
#include "scene_cache_impl.h"

using namespace uicore;

std::shared_ptr<SceneCache> SceneCache::create(const uicore::GraphicContextPtr &gc, const std::string &shader_path)
{
	return std::make_shared<SceneCacheImpl>(gc, shader_path);
}
