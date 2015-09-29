
#pragma once

#include "model_data_mesh.h"
#include "model_data_bone.h"
#include "model_data_light.h"
#include "model_data_camera.h"
#include "model_data_attachment_point.h"
#include "model_data_particle_emitter.h"
#include "model_data_animation.h"
#include "model_data_texture.h"

/// \brief Data structure describing a model.
class ModelData
{
public:
	std::vector<ModelDataMesh> meshes;
	std::vector<ModelDataTexture> textures;
	std::vector<ModelDataBone> bones;
	std::vector<ModelDataLight> lights;
	std::vector<ModelDataCamera> cameras;
	std::vector<ModelDataAttachmentPoint> attachment_points;
	std::vector<ModelDataParticleEmitter> particle_emitters;
	std::vector<ModelDataAnimation> animations;
	uicore::Vec3f aabb_min, aabb_max;

	static void save(uicore::IODevice &device, std::shared_ptr<ModelData> data);
	static std::shared_ptr<ModelData> load(const std::string &filename);
	static std::shared_ptr<ModelData> load(uicore::IODevice &device);
};
