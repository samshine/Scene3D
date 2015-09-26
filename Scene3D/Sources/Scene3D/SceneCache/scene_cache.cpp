
#include "precomp.h"
#include "Scene3D/scene_cache.h"
#include "scene_cache_impl.h"

namespace uicore
{
	SceneCache::SceneCache()
	{
	}

	SceneCache::SceneCache(GraphicContext &gc, const std::string &shader_path) : impl(std::make_shared<SceneCacheImpl>(gc, shader_path))
	{
	}
}
