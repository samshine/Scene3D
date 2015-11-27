
#pragma once

#include "model_instances_buffer.h"
#include "model_shader_cache.h"

class SceneImpl;
class SceneObjectImpl;
class SceneLightProbeImpl;
class ModelLOD;

class ModelRender
{
public:
	void clear(SceneImpl *scene);
	void add_instance(SceneObjectImpl *object);
	void upload(const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection);
	void render_gbuffer();
	void render_transparency();
	void render_shadow();
	void render_early_z();

	int new_model_index();

	ModelShaderCache shader_cache;

private:
	SceneLightProbeImpl *find_nearest_probe(SceneImpl *scene, const uicore::Vec3f &position);

	ModelInstancesBuffer instances_buffer;

	SceneImpl *scene = nullptr;
	std::function<void()> execute_commands;

	int frame = 0;
	std::vector<ModelLOD *> model_meshes;
};
