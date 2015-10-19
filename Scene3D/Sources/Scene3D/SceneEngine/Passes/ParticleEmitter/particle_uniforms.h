
#pragma once

class ParticleUniforms
{
public:
	uicore::Mat4f object_to_eye;
	uicore::Mat4f eye_to_projection;
	float rcp_depth_fade_distance = 0.0f;
	int instance_vectors_offset = 0;
	float padding1 = 0.0f;
	float padding2 = 0.0f;
};
