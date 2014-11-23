
#pragma once

#include <memory>
#include <string>
#include <vector>

namespace clan
{
	class ModelData;
	class FBXAnimation;
	class FBXAttachmentPoint;
	class FBXParticleEmitter;
	class FBXModelImpl;

	class FBXModel
	{
	public:
		FBXModel(const std::string &filename);

		std::shared_ptr<ModelData> convert(const std::vector<FBXAnimation> &animations, const std::vector<FBXAttachmentPoint> &attachment_points, const std::vector<FBXParticleEmitter> &emitters);

	private:
		std::shared_ptr<FBXModelImpl> impl;
	};
}
