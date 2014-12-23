
#pragma once

#include <vector>
#include "fbx_animation.h"
#include "fbx_attachment_point.h"
#include "fbx_particle_emitter.h"
#include "fbx_material.h"

namespace clan
{
	class FBXModelDesc
	{
	public:
		FBXModelDesc();

		std::string fbx_filename;
		std::vector<FBXAnimation> animations;
		std::vector<FBXAttachmentPoint> attachment_points;
		std::vector<FBXParticleEmitter> emitters;
		std::vector<FBXMaterial> materials;

		static FBXModelDesc load(const std::string &filename);
		void save(const std::string &filename);
	};
}
