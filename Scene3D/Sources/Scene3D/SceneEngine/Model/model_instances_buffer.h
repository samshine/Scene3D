
#pragma once

#include "../mapped_buffer.h"

class SceneImpl;
class SceneLightProbeImpl;
class ModelLOD;

class ModelInstancesBuffer
{
public:
	ModelInstancesBuffer();

	int new_offset_index();

	void render_pass(
		const uicore::GraphicContextPtr &gc,
		SceneImpl *scene,
		const uicore::Mat4f &world_to_eye,
		const uicore::Mat4f &eye_to_projection,
		uicore::FrustumPlanes frustum,
		const std::function<void(ModelLOD *model_lod, int num_instances)> &pass_callback);

private:
	SceneLightProbeImpl *find_nearest_probe(SceneImpl *scene, const uicore::Vec3f &position);

	void clear();
	void add(int vectors_count);

	void lock(const uicore::GraphicContextPtr &gc);
	MappedBuffer<uicore::Vec4f> upload(int offset_index, int vectors);
	void unlock(const uicore::GraphicContextPtr &gc);

	uicore::Texture2DPtr &get_indexes() { return indexes[current_buffer]; }
	uicore::Texture2DPtr &get_vectors() { return vectors[current_buffer]; }

	enum { num_buffers = 4 };
	uicore::StagingTexturePtr indexes_transfer[num_buffers];
	uicore::StagingTexturePtr vectors_transfer[num_buffers];

	int frame = 0;

	int max_offset_indexes;
	int next_offset_index;

	int max_vectors;
	int num_vectors;

	int current_buffer;
	uicore::Texture2DPtr indexes[num_buffers];
	uicore::Texture2DPtr vectors[num_buffers];

	friend class ModelLOD;
};
