
#include "precomp.h"
#include "Scene3D/scene_cache.h"
#include "scene_cache_impl.h"

using namespace uicore;

SceneCache::SceneCache()
{
}

SceneCache::SceneCache(const GraphicContextPtr &gc, const std::string &shader_path) : impl(std::make_shared<SceneCacheImpl>(gc, shader_path))
{
}
