
#pragma once

#include "Scene3D/ModelData/model_data.h"
#include "Scene3D/SceneEngine/resource.h"
#include "model_mesh.h"
#include "model_shader_cache.h"
#include <unordered_map>
#include <deque>

class SceneImpl;
class SceneObjectImpl;
class SceneLightProbeImpl;
class ModelMesh;

enum class ModelRenderMode
{
	early_z,
	shadow,
	gbuffer,
	transparency
};

class ModelRender
{
public:
	ModelRender();

	void begin(SceneImpl *scene, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection, ModelRenderMode render_mode);
	void render(SceneObjectImpl *object);
	void end();

private:
	struct InstanceBatch
	{
		ModelMesh *mesh = nullptr;
		int start = 0;
		int count = 0;
		SceneObjectImpl **objects = nullptr;
	};

	void render_batches(const std::vector<InstanceBatch> &batches);
	uicore::Texture2DPtr upload_instance_data(const std::vector<InstanceBatch> &batches);
	void write_instance_data(ModelMesh *mesh, SceneObjectImpl *object, uicore::Vec4f *vectors);
	SceneLightProbeImpl *find_nearest_probe(const uicore::Vec3f &position);

	void render_mesh(ModelMesh *mesh, uicore::Texture2DPtr instance_buffer, int instance_base, int instance_count);
	void render_early_z(ModelMesh *mesh, uicore::Texture2DPtr instance_buffer, uicore::UniformVector<ModelRenderUniforms> &render_uniforms, int instance_count);
	void render_shadow(ModelMesh *mesh, uicore::Texture2DPtr instance_buffer, uicore::UniformVector<ModelRenderUniforms> &render_uniforms, int instance_count);
	void render_gbuffer(ModelMesh *mesh, uicore::Texture2DPtr instance_buffer, uicore::UniformVector<ModelRenderUniforms> &render_uniforms, int instance_count);
	void render_transparency(ModelMesh *mesh, uicore::Texture2DPtr instance_buffer, uicore::UniformVector<ModelRenderUniforms> &render_uniforms, int instance_count);

	void bind_texture(int bind_index, Resource<uicore::TexturePtr> texture, uicore::TextureWrapMode wrap_u, uicore::TextureWrapMode wrap_v);
	void draw_elements(int start_element, int num_elements, uicore::UniformVector<ModelMaterialUniforms> &uniforms, uicore::UniformVector<ModelRenderUniforms> &render_uniforms, int num_instances);

	static uicore::TextureWrapMode to_wrap_mode(ModelDataTextureMap::WrapMode mode);

	uicore::Texture2DPtr get_instance_buffer(int size);
	uicore::StagingTexturePtr get_staging_buffer(int size);
	uicore::UniformVector<ModelRenderUniforms> get_uniform_buffer();

	SceneImpl *scene = nullptr;
	uicore::Mat4f world_to_eye;
	uicore::Mat4f eye_to_projection;
	ModelRenderMode render_mode = ModelRenderMode::early_z;
	std::unordered_map<ModelMesh *, std::vector<SceneObjectImpl *>> render_lists;

	ModelShaderCache shader_cache;

	const int num_vectors = 64 * 1024;
};
