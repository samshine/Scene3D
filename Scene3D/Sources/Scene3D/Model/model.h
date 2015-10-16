
#pragma once

#include "Scene3D/ModelData/model_data.h"
#include "model_instance.h"

class ModelDataLight;
class ModelMaterialCache;
class ModelShaderCache;
class ModelLightInstance;
class ModelMeshVisitor;
class ModelLOD;
class InstancesBuffer;

class Model
{
public:
	Model(const uicore::GraphicContextPtr &gc, ModelMaterialCache &texture_cache, ModelShaderCache &shader_cache, std::shared_ptr<ModelData> model_data, int model_index);
	const std::vector<ModelDataLight> &get_lights();
	const std::shared_ptr<ModelData> &get_model_data() const { return model_data; }

	bool add_instance(int frame, const ModelInstance &instance, const uicore::Mat4f &object_to_world, const uicore::Vec3f &light_probe_color);

	int get_instance_vectors_count() const;
	int get_vectors_per_instance() const;
	void upload(InstancesBuffer &instances_buffer, const uicore::Mat4f &world_to_eye, const uicore::Mat4f &eye_to_projection);

	static const int vectors_per_bone = 3;
	static const int instance_base_vectors = 16;
	static const int vectors_per_material = 14;

private:
	ModelShaderCache &shader_cache;
	std::shared_ptr<ModelData> model_data;
	std::vector<Resource<uicore::TexturePtr> > textures;

	std::vector<std::shared_ptr<ModelLOD> > levels;

	int frame;
	std::vector<const ModelInstance *> instances;
	std::vector<uicore::Mat4f> instances_object_to_world;
	std::vector<uicore::Vec3f> instances_light_probe_color;

	uicore::PixelBufferPtr instance_bones_transfer;
	uicore::Texture2DPtr instance_bones;
	int max_instances;

	int model_index;

	friend class ModelShaderCache;
	friend class InstancesBuffer;
};
