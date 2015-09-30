
#include "precomp.h"
#include "model_cache.h"
#include "Scene3D/SceneCache/instances_buffer.h"
#include "Scene3D/scene_impl.h"
#include "Scene3D/SceneCache/scene_cache_impl.h"

using namespace uicore;

ModelCache::ModelCache(Scene_Impl *scene, ModelMaterialCache &texture_cache, ModelShaderCache &shader_cache, InstancesBuffer &instances_buffer)
: scene(scene), texture_cache(texture_cache), shader_cache(shader_cache), instances_buffer(instances_buffer)
{
}

std::shared_ptr<Model> ModelCache::get_model(const std::string &model_name)
{
	std::shared_ptr<Model> renderer = models[model_name];
	if (!renderer)
	{
		renderer = std::shared_ptr<Model>(new Model(scene->get_cache()->get_gc(), texture_cache, shader_cache, scene->get_cache()->get_model_data(model_name), instances_buffer.new_offset_index()));
		models[model_name]= renderer;
	}
	return renderer;
}
