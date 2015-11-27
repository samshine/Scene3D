
#pragma once

#include "model_render_command.h"
#include "model_mesh_buffers.h"
#include "Scene3D/ModelData/model_data.h"

class ModelData;
class ModelInstance;

class ModelMesh
{
public:
	ModelMesh(SceneEngineImpl *engine, const uicore::GraphicContextPtr &gc, int model_index, std::shared_ptr<ModelData> model_data);

	bool add_instance(int frame, const ModelInstance &instance, const uicore::Mat4f &object_to_world, const uicore::Vec3f &light_probe_color);

	int get_instance_vectors_count() const;
	int get_vectors_per_instance() const;
	void upload(ModelInstancesBuffer &model_instances_buffer, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection);

	static const int vectors_per_bone = 3;
	static const int instance_base_vectors = 16;
	static const int vectors_per_material = 14;

	std::vector<ModelMeshBuffers> mesh_buffers;
	std::vector<Resource<uicore::TexturePtr> > textures;

	ModelRenderCommandList gbuffer_commands;
	ModelRenderCommandList transparency_commands;
	ModelRenderCommandList shadow_commands;
	ModelRenderCommandList early_z_commands;

private:
	template<typename Type>
	uicore::VertexArrayVector<Type> upload_vector(const uicore::GraphicContextPtr &gc, const uicore::PrimitivesArrayPtr &primitives_array, int index, const std::vector<Type> &vec);

	template<typename Type>
	uicore::VertexArrayVector<Type> upload_vector(const uicore::GraphicContextPtr &gc, const uicore::PrimitivesArrayPtr &primitives_array, int index, const std::vector<Type> &vec, bool normalize);

	void create_gbuffer_commands(const uicore::GraphicContextPtr &gc, ModelShaderCache *shader_cache);
	void create_transparency_commands(const uicore::GraphicContextPtr &gc, ModelShaderCache *shader_cache);
	void create_shadow_commands(const uicore::GraphicContextPtr &gc, ModelShaderCache *shader_cache);
	void create_early_z_commands(const uicore::GraphicContextPtr &gc, ModelShaderCache *shader_cache);

	static uicore::TextureWrapMode to_wrap_mode(ModelDataTextureMap::WrapMode mode);

	int model_index;
	std::shared_ptr<ModelData> model_data;

	int frame = -1;
	std::vector<const ModelInstance *> instances;
	std::vector<uicore::Mat4f> instances_object_to_world;
	std::vector<uicore::Vec3f> instances_light_probe_color;

	uicore::PixelBufferPtr instance_bones_transfer;
	uicore::Texture2DPtr instance_bones;
	int max_instances = 0;

	friend class ModelRender;
};
