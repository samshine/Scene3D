
#pragma once

namespace uicore
{
	class ModelDescParticleEmitter
	{
	public:
		Vec3f position;
		int bone_selector;
		float size;
		float speed;
		float spread;
		float gravity;
		float longevity;
		float delay;
		Vec4f color;
		std::string texture;
	};
}
