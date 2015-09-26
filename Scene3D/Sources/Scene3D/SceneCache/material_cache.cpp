
#include "precomp.h"
#include "material_cache.h"
#include "scene_cache_impl.h"
#include "Scene3D/scene_impl.h"

using namespace uicore;

MaterialCache::MaterialCache(Scene_Impl *scene)
: scene(scene)
{
}

void MaterialCache::update(GraphicContext &gc, float time_elapsed)
{
	scene->get_cache()->update_textures(gc, time_elapsed);
}

Resource<Texture> MaterialCache::get_texture(GraphicContext &gc, const std::string &material_name, bool linear)
{
	return scene->get_cache()->get_texture(gc, material_name, linear);
}
